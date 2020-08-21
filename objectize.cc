#include <emscripten.h>
#include "geometry.h"
#include "compose.h"
#include "noise.h"
#include "march.h"
#include "collide.h"
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
  void queue(M *message) {
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
  void push(M *message) {
    std::lock_guard<std::mutex> lock(mutex);
    messages.push_back(message);
    // semaphore.release();
  }
  M *pop() {
    M *message;
    {
      std::lock_guard<std::mutex> lock(mutex);
      if (messages.size() > 0) {
        message = messages.front();
        messages.pop_front();
        // semaphore.acquire();
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
          freeList[i] = new FreeEntry{
            entry->start + size,
            entry->count - size,
          };
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

// memory

EMSCRIPTEN_KEEPALIVE void *doMalloc(size_t size) {
  return malloc(size);
}

EMSCRIPTEN_KEEPALIVE void doFree(void *ptr) {
  free(ptr);
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

// vegetation

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

EMSCRIPTEN_KEEPALIVE void getMarchObjectStats(GeometrySet *geometrySet, Subparcel *subparcel, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIds, unsigned int *numIndices, unsigned int *numSkyLights, unsigned int *numTorchLights) {
  doGetMarchObjectStats(geometrySet, subparcel, *numPositions, *numUvs, *numIds, *numIndices, *numSkyLights, *numTorchLights);
}

EMSCRIPTEN_KEEPALIVE void marchObjects(GeometrySet *geometrySet, int x, int y, int z, Subparcel *subparcel, Subparcel *subparcels, unsigned int numSubparcels, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int indexOffset) {
  doMarchObjects(geometrySet, x, y, z, subparcel, subparcels, numSubparcels, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);
}

// land

EMSCRIPTEN_KEEPALIVE float doGetHeight(int seed, float ax, float ay, float az, float baseHeight) {
  return getHeight(seed, ax, ay, az, baseHeight);
}

EMSCRIPTEN_KEEPALIVE void doNoise3(int seed, int x, int y, int z, float baseHeight, float wormRate, float wormRadiusBase, float wormRadiusRate, float objectsRate, float potentialDefault, void *subparcelByteOffset) {
  noise3(seed, x, y, z, baseHeight, wormRate, wormRadiusBase, wormRadiusRate, objectsRate, potentialDefault, subparcelByteOffset);
}

EMSCRIPTEN_KEEPALIVE void doMarchingCubes2(float meshId, int dims[3], float *potential, unsigned char *biomes, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float *positions, float *normals, float *uvs, float *barycentrics, unsigned char *aos, float *ids, unsigned char *skyLights, unsigned char *torchLights, unsigned int *positionIndex, unsigned int *normalIndex, unsigned int *uvIndex, unsigned int *barycentricIndex, unsigned int *aoIndex, unsigned int *idIndex, unsigned int *skyLightsIndex, unsigned int *torchLightsIndex, unsigned int &numOpaquePositions, unsigned int &numTransparentPositions, unsigned char *peeks) {
  marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions, normals, uvs, barycentrics, aos, ids, skyLights, torchLights, *positionIndex, *normalIndex, *uvIndex, *barycentricIndex, *aoIndex, *idIndex, *skyLightsIndex, *torchLightsIndex, numOpaquePositions, numTransparentPositions, peeks);
}

// physics

EMSCRIPTEN_KEEPALIVE void initPhysx() {
  doInitPhysx();
}
EMSCRIPTEN_KEEPALIVE void registerBakedGeometry(unsigned int meshId, PxDefaultMemoryOutputStream *writeStream, float *meshPosition, float *meshQuaternion, uintptr_t *result) {
  *result = doRegisterBakedGeometry(meshId, writeStream, meshPosition, meshQuaternion);
}
EMSCRIPTEN_KEEPALIVE void registerBoxGeometry(unsigned int meshId, float *position, float *quaternion, float w, float h, float d, uintptr_t *result) {
  *result = doRegisterBoxGeometry(meshId, position, quaternion, w, h, d);
}
EMSCRIPTEN_KEEPALIVE void registerCapsuleGeometry(unsigned int meshId, float *position, float *quaternion, float radius, float halfHeight, uintptr_t *result) {
  *result = doRegisterCapsuleGeometry(meshId, position, quaternion, radius, halfHeight);
}
EMSCRIPTEN_KEEPALIVE void bakeGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, PxDefaultMemoryOutputStream **writeStream) {
  *writeStream = doBakeGeometry(positions, indices, numPositions, numIndices);
}
EMSCRIPTEN_KEEPALIVE void releaseBakedGeometry(PxDefaultMemoryOutputStream *writeStream) {
  delete writeStream;
}
EMSCRIPTEN_KEEPALIVE void unregisterGeometry(uintptr_t geometrySpecPtr) {
  doUnregisterGeometry(geometrySpecPtr);
}
EMSCRIPTEN_KEEPALIVE void raycast(float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int *hit, float *position, float *normal, float *distance, unsigned int *meshId, unsigned int *faceIndex) {
  doRaycast(origin, direction, meshPosition, meshQuaternion, *hit, position, normal, *distance, *meshId, *faceIndex);
}
EMSCRIPTEN_KEEPALIVE void collide(float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int *hit, float *direction, unsigned int *grounded) {
  doCollide(radius, halfHeight, position, quaternion, meshPosition, meshQuaternion, maxIter, *hit, direction, *grounded);
}

EMSCRIPTEN_KEEPALIVE void *makeCuller() {
  return doMakeCuller();
}
EMSCRIPTEN_KEEPALIVE GroupSet *registerGroupSet(Culler *culler, int x, int y, int z, float r, unsigned char *peeks, Group *groups, unsigned int numGroups) {
  return doRegisterGroupSet(culler, x, y, z, r, peeks, groups, numGroups);
}
EMSCRIPTEN_KEEPALIVE void unregisterGroupSet(Culler *culler, GroupSet *groupSet) {
  doUnregisterGroupSet(culler, groupSet);
}
EMSCRIPTEN_KEEPALIVE void cull(Culler *culler, float *positionData, float *matrixData, float slabRadius, CullResult *cullResults, unsigned int *numCullResults) {
  doCull(culler, positionData, matrixData, slabRadius, cullResults, *numCullResults);
}

// requests

EMSCRIPTEN_KEEPALIVE ThreadPool *makeThreadPool(unsigned int numThreads) {
  return new ThreadPool(numThreads);
}

unsigned int nextMessageId = 0;
EMSCRIPTEN_KEEPALIVE unsigned int pushRequest(ThreadPool *threadPool, RequestMessage *message) {
  unsigned int messageId = ++nextMessageId;
  message->id = messageId;
  threadPool->inbox.queue(message);
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

    Subparcel *subparcel = *((Subparcel **)(requestMessage->args + index));
    index += sizeof(Subparcel *);
    Subparcel *subparcels = *((Subparcel **)(requestMessage->args + index));
    index += sizeof(Subparcel *);
    unsigned int numSubparcels = *((unsigned int *)(requestMessage->args + index));
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
    doGetMarchObjectStats(geometrySet, subparcel, numPositions, numUvs, numIds, numIndices, numSkyLights, numTorchLights);

    // std::cout << "march objects b " << numPositions << " " << numUvs << " " << numIds << " " << numIndices << " " << numSkyLights << " " << numTorchLights << " " << (unsigned int)(void *)positionsAllocator << std::endl;

    *positionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
    if (!*positionsEntry) {
      std::cout << "could not allocate marchObjects positions" << std::endl;
      abort();
    }
    *uvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
    if (!*uvsEntry) {
      std::cout << "could not allocate marchObjects uvs" << std::endl;
      abort();
    }
    *idsEntry = idsAllocator->alloc(numIds*sizeof(float));
    if (!*idsEntry) {
      std::cout << "could not allocate marchObjects ids" << std::endl;
      abort();
    }
    *indicesEntry = indicesAllocator->alloc(numIndices*sizeof(unsigned int));
    if (!*indicesEntry) {
      std::cout << "could not allocate marchObjects indices" << std::endl;
      abort();
    }
    *skyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
    if (!*skyLightsEntry) {
      std::cout << "could not allocate marchObjects skyLights" << std::endl;
      abort();
    }
    *torchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
    if (!*torchLightsEntry) {
      std::cout << "could not allocate marchObjects torchLights" << std::endl;
      abort();
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

    doMarchObjects(geometrySet, x, y, z, subparcel, subparcels, numSubparcels, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);

    // std::cout << "march objects d " << numIndices << " " << indices[0] << " " << indices[1] << " " << indices[2] << " " << (unsigned int)(void *)positionsAllocator << " " << (unsigned int)(void *)indicesAllocator << " " << (unsigned int)(void *)indicesAllocator->data << " " << (*indicesEntry)->start << std::endl;
  },
  [](RequestMessage *requestMessage) -> void { // getHeight
    unsigned int index = 0;
    int seed = *((int *)(requestMessage->args + index));
    index += sizeof(int);
    float x = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float y = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float z = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float baseHeight = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float *height = (float *)(requestMessage->args + index);
    index += sizeof(float *);
    *height = getHeight(seed, x, y, z, baseHeight);
  },
  [](RequestMessage *requestMessage) -> void { // noise
    unsigned int index = 0;

    int seed = *((int *)(requestMessage->args + index));
    index += sizeof(int);

    float x = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float y = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float z = *((float *)(requestMessage->args + index));
    index += sizeof(float);

    float baseHeight = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float wormRate = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float wormRadiusBase = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float wormRadiusRate = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float objectsRate = *((float *)(requestMessage->args + index));
    index += sizeof(float);
    float potentialDefault = *((float *)(requestMessage->args + index));
    index += sizeof(float);

    void *subparcelByteOffset = *((void **)(requestMessage->args + index));
    index += sizeof(void *);

    noise3(seed, x, y, z, baseHeight, wormRate, wormRadiusBase, wormRadiusRate, objectsRate, potentialDefault, subparcelByteOffset);
  },
  [](RequestMessage *requestMessage) -> void { // marchingCubes
    unsigned int index = 0;

    float meshId = *(float *)(requestMessage->args + index);
    index += sizeof(float);

    int *dims = (int *)(requestMessage->args + index);
    index += 3*sizeof(int);

    float *potential = *((float **)(requestMessage->args + index));
    index += sizeof(float *);
    unsigned char *biomes = *((unsigned char **)(requestMessage->args + index));
    index += sizeof(unsigned char *);
    char *heightfield = *((char **)(requestMessage->args + index));
    index += sizeof(char *);
    unsigned char *lightfield = *((unsigned char **)(requestMessage->args + index));
    index += sizeof(unsigned char *);

    float *shift = (float *)(requestMessage->args + index);
    index += 3*sizeof(float);
    float *scale = (float *)(requestMessage->args + index);
    index += 3*sizeof(float);

    ArenaAllocator *positionsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *normalsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *uvsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *barycentricsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *aosAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *idsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *skyLightsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *torchLightsAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *peeksAllocator = *((ArenaAllocator **)(requestMessage->args + index));
    index += sizeof(ArenaAllocator *);

    FreeEntry **positionsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **normalsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **uvsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **barycentricsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **aosEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **idsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **skyLightsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **torchLightsEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **peeksEntry = (FreeEntry **)(requestMessage->args + index);
    index += sizeof(FreeEntry *);

    unsigned int *numOpaquePositions = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numTransparentPositions = (unsigned int *)(requestMessage->args + index);
    index += sizeof(unsigned int *);

    constexpr int bufferSize = 1024*1024;
    std::vector<float> positions(bufferSize);
    std::vector<float> normals(bufferSize);
    std::vector<float> uvs(bufferSize);
    std::vector<float> barycentrics(bufferSize);
    std::vector<unsigned char> aos(bufferSize);
    std::vector<float> ids(bufferSize);
    std::vector<unsigned char> skyLights(bufferSize);
    std::vector<unsigned char> torchLights(bufferSize);
    constexpr unsigned int numPeeks = 15;
    std::vector<unsigned char> peeks(numPeeks);
    unsigned int numPositions;
    unsigned int numNormals;
    unsigned int numUvs;
    unsigned int numBarycentrics;
    unsigned int numAos;
    unsigned int numIds;
    unsigned int numSkyLights;
    unsigned int numTorchLights;
    marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions.data(), normals.data(), uvs.data(), barycentrics.data(), aos.data(), ids.data(), skyLights.data(), torchLights.data(), numPositions, numNormals, numUvs, numBarycentrics, numAos, numIds, numSkyLights, numTorchLights, *numOpaquePositions, *numTransparentPositions, peeks.data());

    *positionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
    if (!*positionsEntry) {
      std::cout << "could not allocate marchingCubes positions" << std::endl;
      abort();
    }
    *normalsEntry = normalsAllocator->alloc(numNormals*sizeof(float));
    if (!*normalsEntry) {
      std::cout << "could not allocate marchingCubes normals" << std::endl;
      abort();
    }
    *uvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
    if (!*uvsEntry) {
      std::cout << "could not allocate marchingCubes uvs" << std::endl;
      abort();
    }
    *barycentricsEntry = barycentricsAllocator->alloc(numBarycentrics*sizeof(float));
    if (!*barycentricsEntry) {
      std::cout << "could not allocate marchingCubes barycentrics" << std::endl;
      abort();
    }
    *aosEntry = aosAllocator->alloc(numAos*sizeof(float));
    if (!*aosEntry) {
      std::cout << "could not allocate marchingCubes aos" << std::endl;
      abort();
    }
    *idsEntry = idsAllocator->alloc(numIds*sizeof(float));
    if (!*idsEntry) {
      std::cout << "could not allocate marchingCubes ids" << std::endl;
      abort();
    }
    *skyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
    if (!*skyLightsEntry) {
      std::cout << "could not allocate marchingCubes skyLights" << std::endl;
      abort();
    }
    *torchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
    if (!*torchLightsEntry) {
      std::cout << "could not allocate marchingCubes torchLights" << std::endl;
      abort();
    }
    *peeksEntry = peeksAllocator->alloc(numPeeks*sizeof(unsigned char));
    if (!*peeksEntry) {
      std::cout << "could not allocate marchingCubes peeks" << std::endl;
      abort();
    }

    memcpy(positionsAllocator->data + (*positionsEntry)->start, positions.data(), numPositions*sizeof(float));
    memcpy(normalsAllocator->data + (*normalsEntry)->start, normals.data(), numNormals*sizeof(float));
    memcpy(uvsAllocator->data + (*uvsEntry)->start, uvs.data(), numUvs*sizeof(float));
    memcpy(barycentricsAllocator->data + (*barycentricsEntry)->start, barycentrics.data(), numBarycentrics*sizeof(float));
    memcpy(aosAllocator->data + (*aosEntry)->start, aos.data(), numAos*sizeof(unsigned char));
    memcpy(idsAllocator->data + (*idsEntry)->start, ids.data(), numIds*sizeof(float));
    memcpy(skyLightsAllocator->data + (*skyLightsEntry)->start, skyLights.data(), numSkyLights*sizeof(unsigned char));
    memcpy(torchLightsAllocator->data + (*torchLightsEntry)->start, torchLights.data(), numTorchLights*sizeof(unsigned char));
    memcpy(peeksAllocator->data + (*peeksEntry)->start, peeks.data(), numPeeks*sizeof(unsigned char));
  },
  [](RequestMessage *requestMessage) -> void { // bakeGeometry
    unsigned int index = 0;

    float *positions = *((float **)(requestMessage->args + index));
    index += sizeof(float *);

    unsigned int *indices = *((unsigned int **)(requestMessage->args + index));
    index += sizeof(unsigned int *);

    unsigned int numPositions = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);

    unsigned int numIndices = *((unsigned int *)(requestMessage->args + index));
    index += sizeof(unsigned int);

    PxDefaultMemoryOutputStream **writeStream = (PxDefaultMemoryOutputStream **)(requestMessage->args + index);
    index += sizeof(PxDefaultMemoryOutputStream *);

    // std::cout << "bake 1" << std::endl;
    bakeGeometry(positions, indices, numPositions, numIndices, writeStream);
    // std::cout << "bake 2" << std::endl;
  },
};

}