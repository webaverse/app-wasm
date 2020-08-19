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

enum class Method : unsigned int {
  makeGeometrySet = 0,
  loadBake,
  getGeometry,
  getAnimalGeometry,
  marchObjects,
};

class RequestMessage {
public:
  unsigned int id;
  Method method;
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
          auto &fn = METHOD_FNS[(unsigned int)requestMessage->method];
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

EMSCRIPTEN_KEEPALIVE void *makeGeometrySet() {
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

EMSCRIPTEN_KEEPALIVE void marchObjects(GeometrySet *geometrySet, int x, int y, int z, MarchObject *marchObjects, unsigned int numMarchObjects, SubparcelObject *subparcelObjects, unsigned int numSubparcelObjects, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIds, unsigned int *numIndices, unsigned int *numSkyLights, unsigned int *numTorchLights) {
  doMarchObjects(geometrySet, x, y, z, marchObjects, numMarchObjects, subparcelObjects, numSubparcelObjects, positions, uvs, ids, indices, skyLights, torchLights, *numPositions, *numUvs, *numIds, *numIndices, *numSkyLights, *numTorchLights);
}

EMSCRIPTEN_KEEPALIVE void doFree(void *ptr) {
  free(ptr);
}

std::function<void(RequestMessage *)> METHOD_FNS[] = {
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
};

}