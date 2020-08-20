#include <emscripten.h>
#include "geometry.h"
// #include <iostream>

#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

class Semaphore
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_ = 0; // Initialized as locked.

public:
    void release() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

    void acquire() {
        std::unique_lock<decltype(mutex_)> lock(mutex_);
        while(!count_) // Handle spurious wake-ups.
            condition_.wait(lock);
        --count_;
    }

    bool try_acquire() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        if(count_) {
            --count_;
            return true;
        }
        return false;
    }
};

class RequestMessage {
public:
  unsigned int id;
  unsigned int method;
  unsigned char args[4];
};
class ResponseMessage {
public:
  unsigned int id;
};

template <typename M>
class Mailbox {
public:
  void push(M *message) {
    std::lock_guard<std::mutex> lock(mutex);
    messages.push_back(message);
    semaphore.release();
  }
  M *wait() {
    semaphore.acquire();
    M *message;
    {
      std::lock_guard<std::mutex> lock(mutex);
      message = messages.front();
      messages.pop_front();
    }
    return message;
  }
  M *pop() {
    M *message;
    {
      std::lock_guard<std::mutex> lock(mutex);
      if (messages.size() > 0) {
        message = messages.front();
        messages.pop_front();
        semaphore.acquire();
      } else {
        message = nullptr;
      }
    }
    return message;
  }

  std::deque<M *> messages;
  Semaphore semaphore;
  std::mutex mutex;
};

extern "C" {
  extern std::function<void(RequestMessage *)> METHOD_FNS[];
}

class FreeEntry {
public:
  unsigned int start;
  unsigned int count;
};
class ArenaAllocator {
public:
  ArenaAllocator(unsigned int size) {
    data = (unsigned char *)malloc(size);
    freeList.push_back(new FreeEntry{
      0,
      size,
    });
  }
  FreeEntry *alloc(unsigned int size) {
    std::lock_guard<std::mutex> lock(mutex);

    for (unsigned int i = 0; i < freeList.size(); i++) {
      FreeEntry *entry = freeList[i];
      if (entry->count >= size) {
        if (entry->count > size) {
          FreeEntry *nextEntry = new FreeEntry{
            entry->start + size,
            entry->count - size,
          };
          freeList[i] = nextEntry;
        } else {
          freeList.erase(freeList.begin() + i);
        }
        entry->count = size;
        return entry;
      }
    }
    return nullptr;
  }
  void free(FreeEntry *freeEntry) {
    std::lock_guard<std::mutex> lock(mutex);

    freeList.push_back(freeEntry);
    updateFreeList();
  }
  void updateFreeList() {
    std::sort(freeList.begin(), freeList.end(), [](FreeEntry *a, FreeEntry *b) -> bool {
      return a->start < b->start;
    });
    bool merged = false;
    for (unsigned int i = 0; i < freeList.size()-1; i++) {
      FreeEntry *entry = freeList[i];
      if (entry) {
        for (unsigned int j = i+1; j < freeList.size(); j++) {
          FreeEntry *nextEntry = freeList[j];
          if (nextEntry) {
            if (entry->start + entry->count == nextEntry->start) {
              entry->count += nextEntry->count;
              freeList[j] = nullptr;
              delete nextEntry;
              merged = true;
            }
          }
        }
      }
    }
    if (merged) {
      freeList.erase(std::remove_if(freeList.begin(), freeList.end(), [](FreeEntry *freeEntry) {
        return freeEntry == nullptr;
      }), freeList.end());
    }
  }
  unsigned char *data;
  std::vector<FreeEntry *> freeList;
  std::mutex mutex;
};

class ThreadPool {
public:
  ThreadPool(unsigned int numThreads) {
    for (unsigned int i = 0; i < numThreads; i++) {
      // std::cout << "starting thread " << i << std::endl;
      std::thread thread([this]() -> void {
        // std::cout << "thread pool running" << std::endl;
        for (;;) {
          RequestMessage *requestMessage = inbox.wait();
          // std::cout << "got request message method a " << (void *)requestMessage << " " << (unsigned int)requestMessage->method << std::endl;
          auto &fn = METHOD_FNS[requestMessage->method];
          // std::cout << "got request message method b" << std::endl;
          fn(requestMessage);
          // std::cout << "got message " << (unsigned int)message->method << std::endl;
          outbox.push(new ResponseMessage{requestMessage->id});
        }
      });
      thread.detach();
    }
  }
  Mailbox<RequestMessage> inbox;
  Mailbox<ResponseMessage> outbox;
};

extern "C" {

EMSCRIPTEN_KEEPALIVE ThreadPool *makeThreadPool(unsigned int numThreads) {
  return new ThreadPool(numThreads);
}

unsigned int nextMessageId = 0;
EMSCRIPTEN_KEEPALIVE unsigned int pushRequest(ThreadPool *threadPool, RequestMessage *message) {
  unsigned int messageId = ++nextMessageId;
  message->id = messageId;
  threadPool->inbox.push(message);
  return messageId;
}

EMSCRIPTEN_KEEPALIVE unsigned int popResponse(ThreadPool *threadPool) {
  ResponseMessage *responseMessage = threadPool->outbox.pop();
  if (responseMessage) {
    unsigned int messageId = responseMessage->id;
    delete responseMessage;
    return messageId;
  } else {
    return 0;
  }
}

EMSCRIPTEN_KEEPALIVE ArenaAllocator *makeArenaAllocator(unsigned int size) {
  return new ArenaAllocator(size);
}

EMSCRIPTEN_KEEPALIVE FreeEntry *arenaAlloc(ArenaAllocator *arenaAllocator, unsigned int size) {
  return arenaAllocator->alloc(size);
}

EMSCRIPTEN_KEEPALIVE void arenaFree(ArenaAllocator *arenaAllocator, FreeEntry *entry) {
  arenaAllocator->free(entry);
}

EMSCRIPTEN_KEEPALIVE GeometrySet *makeGeometrySet() {
  return doMakeGeometrySet();
}

EMSCRIPTEN_KEEPALIVE void loadBake(GeometrySet *geometrySet, unsigned char *data, unsigned int size) {
  doLoadBake(geometrySet, data, size);
}

EMSCRIPTEN_KEEPALIVE void getGeometry(GeometrySet *geometrySet, char *nameData, unsigned int nameSize, float **positions, float **uvs, unsigned int **indices, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIndices) {
  doGetGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, *numPositions, *numUvs, *numIndices);
}

EMSCRIPTEN_KEEPALIVE void getAnimalGeometry(GeometrySet *geometrySet, unsigned int hash, float **positions, unsigned char **colors, unsigned int **indices, float **heads, float **legs, unsigned int *numPositions, unsigned int *numColors, unsigned int *numIndices, unsigned int *numHeads, unsigned int *numLegs, float *headPivot, float *aabb) {
  doGetAnimalGeometry(geometrySet, hash, positions, colors, indices, heads, legs, *numPositions, *numColors, *numIndices, *numHeads, *numLegs, headPivot, aabb);
}

EMSCRIPTEN_KEEPALIVE void getMarchObjectStats(GeometrySet *geometrySet, MarchObject *marchObjects, unsigned int numMarchObjects, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIds, unsigned int *numIndices, unsigned int *numSkyLights, unsigned int *numTorchLights) {
  doGetMarchObjectStats(geometrySet, marchObjects, numMarchObjects, *numPositions, *numUvs, *numIds, *numIndices, *numSkyLights, *numTorchLights);
}

EMSCRIPTEN_KEEPALIVE void marchObjects(GeometrySet *geometrySet, int x, int y, int z, MarchObject *marchObjects, unsigned int numMarchObjects, SubparcelObject *subparcelObjects, unsigned int numSubparcelObjects, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int indexOffset) {
  doMarchObjects(geometrySet, x, y, z, marchObjects, numMarchObjects, subparcelObjects, numSubparcelObjects, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);
}

EMSCRIPTEN_KEEPALIVE void doFree(void *ptr) {
  free(ptr);
}

std::function<void(RequestMessage *)> METHOD_FNS[] = {
  [](RequestMessage *requestMessage) -> void { // makeArenaAllocator
    unsigned int index = 0;
    unsigned int size = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);
    ArenaAllocator **arenaAllocator = (ArenaAllocator **)(requestMessage->args + index);
    index += sizeof(ArenaAllocator *);

    *arenaAllocator = makeArenaAllocator(size);
  },
  [](RequestMessage *requestMessage) -> void { // arenaAlloc
    unsigned int index = 0;
    ArenaAllocator *arenaAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    unsigned int size = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);
    FreeEntry **entry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry **);

    *entry = arenaAlloc(arenaAllocator, size);
  },
  [](RequestMessage *requestMessage) -> void { // arenaFree
    unsigned int index = 0;
    ArenaAllocator *arenaAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    FreeEntry *entry = *((FreeEntry **)(requestMessage->args + index));
    index += sizeof(FreeEntry *);

    arenaFree(arenaAllocator, entry);
  },
  [](RequestMessage *requestMessage) -> void { // makeGeometrySet
    GeometrySet **geometrySet = (GeometrySet **)requestMessage->args;
    *geometrySet = new GeometrySet();
  },
  [](RequestMessage *requestMessage) -> void { // loadBake
    GeometrySet *geometrySet = *((GeometrySet **)requestMessage->args);
    unsigned char *data = *((unsigned char **)(requestMessage->args + sizeof(GeometrySet *)));
    unsigned int dataSize = *((unsigned int *)(requestMessage->args + sizeof(GeometrySet *) + sizeof(unsigned char *)));
    loadBake(geometrySet, data, dataSize);
  },
  [](RequestMessage *requestMessage) -> void { // getGeometry
    unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(requestMessage->args + index));
    index += sizeof(GeometrySet *);
    char *nameData = *((char **)(requestMessage->args + index));
    index += sizeof(char *);
    unsigned int nameSize = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(requestMessage->args + index);
    index += sizeof(float **);
    float **uvs = (float **)(requestMessage->args + index);
    index += sizeof(float **);
    unsigned int **indices = (unsigned int **)(requestMessage->args + index);
    index += sizeof(unsigned int **);
    unsigned int *numPositions = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numUvs = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);

    getGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, numPositions, numUvs, numIndices);
  },
  [](RequestMessage *requestMessage) -> void { // getAnimalGeometry
    unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(requestMessage->args + index));
    index += sizeof(GeometrySet *);
    unsigned int hash = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(requestMessage->args + index);
    index += sizeof(float **);
    unsigned char **colors = (unsigned char **)(requestMessage->args + index);
    index += sizeof(unsigned char **);
    unsigned int **indices = (unsigned int **)(requestMessage->args + index);
    index += sizeof(unsigned int **);
    float **heads = (float **)(requestMessage->args + index);
    index += sizeof(float **);
    float **legs = (float **)(requestMessage->args + index);
    index += sizeof(float **);
    unsigned int *numPositions = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numColors = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numHeads = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numLegs = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    float *headPivot = (float *)(requestMessage->args + index);
    index += 3*sizeof(float *);
    float *aabb = (float *)(requestMessage->args + index);
    index += 6*sizeof(float *);

    getAnimalGeometry(geometrySet, hash, positions, colors, indices, heads, legs, numPositions, numColors, numIndices, numHeads, numLegs, headPivot, aabb);

    /* unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(requestMessage->args + index));
    index += sizeof(GeometrySet *);
    char *nameData = *((char **)(requestMessage->args + index));
    index += sizeof(char *);
    unsigned int nameSize = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(requestMessage->args + index);
    index += sizeof(float **);
    float **uvs = (float **)(requestMessage->args + index);
    index += sizeof(float **);
    unsigned int **indices = (unsigned int **)(requestMessage->args + index);
    index += sizeof(unsigned int **);
    unsigned int *numPositions = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numUvs = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);

    getGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, numPositions, numUvs, numIndices); */
  },
  [](RequestMessage *requestMessage) -> void { // marchObjects
    unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(requestMessage->args + index));
    index += sizeof(GeometrySet *);
    int x = *((int *)(requestMessage->args + index));
    index += sizeof(int);
    int y = *((int *)(requestMessage->args + index));
    index += sizeof(int);
    int z = *((int *)(requestMessage->args + index));
    index += sizeof(int);

    // std::cout << "march objects a " << x << " " << y << " " << z << std::endl;

    MarchObject *marchObjects = *((MarchObject **)(requestMessage->args + index));
    index += sizeof(MarchObject *);
    unsigned int numMarchObjects = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);
    SubparcelObject *subparcelObjects = *((SubparcelObject **)(requestMessage->args + index));
    index += sizeof(SubparcelObject *);
    unsigned int numSubparcelObjects = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);
    ArenaAllocator *positionsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *uvsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *idsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *indicesAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *skyLightsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *torchLightsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    FreeEntry **positionsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **uvsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **idsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **indicesEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **skyLightsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **torchLightsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);

    // std::cout << "march objects ax " << (unsigned int)(void *)geometrySet << " " << (unsigned int)(void *)marchObjects << " " << numMarchObjects << std::endl;

    unsigned int numPositions;
    unsigned int numUvs;
    unsigned int numIds;
    unsigned int numIndices;
    unsigned int numSkyLights;
    unsigned int numTorchLights;
    doGetMarchObjectStats(geometrySet, marchObjects, numMarchObjects, numPositions, numUvs, numIds, numIndices, numSkyLights, numTorchLights);

    // std::cout << "march objects b " << numPositions << " " << numUvs << " " << numIds << " " << numIndices << " " << numSkyLights << " " << numTorchLights << " " << (unsigned int)(void *)positionsAllocator << std::endl;

    *positionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
    if (!*positionsEntry) {
      std::cout << "could not allocate positions" << std::endl;
    }
    *uvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
    if (!*uvsEntry) {
      std::cout << "could not allocate uvs" << std::endl;
    }
    *idsEntry = idsAllocator->alloc(numIds*sizeof(float));
    if (!*idsEntry) {
      std::cout << "could not allocate ids" << std::endl;
    }
    *indicesEntry = indicesAllocator->alloc(numIndices*sizeof(unsigned int));
    if (!*indicesEntry) {
      std::cout << "could not allocate indices" << std::endl;
    }
    *skyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
    if (!*skyLightsEntry) {
      std::cout << "could not allocate skyLights" << std::endl;
    }
    *torchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
    if (!*torchLightsEntry) {
      std::cout << "could not allocate torchLights" << std::endl;
    }

    // std::cout << "march objects c " << numPositions << " " << numUvs << " " << numIds << " " << numIndices << " " << numSkyLights << " " << numTorchLights << std::endl;

    float *positions = (float *)(positionsAllocator->data + (*positionsEntry)->start);
    float *uvs = (float *)(uvsAllocator->data + (*uvsEntry)->start);
    float *ids = (float *)(idsAllocator->data + (*idsEntry)->start);
    unsigned int *indices = (unsigned int *)(indicesAllocator->data + (*indicesEntry)->start);
    unsigned char *skyLights = (unsigned char *)(skyLightsAllocator->data + (*skyLightsEntry)->start);
    unsigned char *torchLights = (unsigned char *)(torchLightsAllocator->data + (*torchLightsEntry)->start);

    /* slab => (slab.position.byteOffset - geometry.attributes.position.array.byteOffset)/Float32Array.BYTES_PER_ELEMENT;
    const indexOffset = vegetationMesh.getSlabPositionOffset(slab)/3;
    for (let i = 0; i < spec.indices.length; i++) {
      spec.indices[i] += indexOffset;
    } */
    unsigned int indexOffset = (*positionsEntry)->start/sizeof(float)/3;

    doMarchObjects(geometrySet, x, y, z, marchObjects, numMarchObjects, subparcelObjects, numSubparcelObjects, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);

    // std::cout << "march objects d " << numIndices << " " << indices[0] << " " << indices[1] << " " << indices[2] << " " << (unsigned int)(void *)positionsAllocator << " " << (unsigned int)(void *)indicesAllocator << " " << (unsigned int)(void *)indicesAllocator->data << " " << (*indicesEntry)->start << std::endl;
  },
};

}