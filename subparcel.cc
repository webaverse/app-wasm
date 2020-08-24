#include "subparcel.h"
#include <iostream>

int absi(int n) {
  return std::abs(n);
}
int sign(int n) {
  return n < 0 ? 1 : 0;
}
int getSubparcelIndex(int x, int y, int z) {
  return absi(x)|(absi(y)<<9)|(absi(z)<<18)|(sign(x)<<27)|(sign(y)<<28)|(sign(z)<<29);
}
unsigned int getFieldIndex(unsigned int x, unsigned int y, unsigned int z) {
  return x + (z * SUBPARCEL_SIZE_P1) + (y * SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1);
}

void Semaphore::release() {
  std::lock_guard<decltype(mutex_)> lock(mutex_);
  ++count_;
  condition_.notify_one();
}
void Semaphore::acquire() {
  std::unique_lock<decltype(mutex_)> lock(mutex_);
  while(!count_) // Handle spurious wake-ups.
    condition_.wait(lock);
  --count_;
}
bool Semaphore::try_acquire() {
  std::lock_guard<decltype(mutex_)> lock(mutex_);
  if(count_) {
    --count_;
    return true;
  }
  return false;
}

ArenaAllocator::ArenaAllocator(unsigned int size) {
  data = (unsigned char *)malloc(size);
  freeList.push_back(new FreeEntry{
    0,
    size,
  });
}
FreeEntry *ArenaAllocator::alloc(unsigned int size) {
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
void ArenaAllocator::free(FreeEntry *freeEntry) {
  std::lock_guard<std::mutex> lock(mutex);

  freeList.push_back(freeEntry);
  updateFreeList();
}
void ArenaAllocator::updateFreeList() {
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

ThreadPool::ThreadPool(unsigned int numThreads) {
  for (unsigned int i = 0; i < numThreads; i++) {
    // std::cout << "starting thread " << i << std::endl;
    std::thread thread([this]() -> void {
      // std::cout << "thread pool running" << std::endl;
      for (;;) {
        Message *message = inbox.wait();
        // std::cout << "got request message method " << (unsigned int)message->method << std::endl;
        auto &fn = METHOD_FNS[message->method];
        // std::cout << "got request message method b" << std::endl;
        fn(message);

        if (message->id != 0) {
          outbox.push(message);
        } /* else {
          std::cout << "ignore message due to id " << message->id << std::endl;
        } */

        // std::cout << "push message " << outbox << " " << outbox.messages.size() << std::endl;
      }
    });
    thread.detach();
  }
}

Tracker::Tracker(int seed, int chunkDistance, ArenaAllocator *positionsAllocator, ArenaAllocator *normalsAllocator, ArenaAllocator *uvsAllocator, ArenaAllocator *barycentricsAllocator, ArenaAllocator *aosAllocator, ArenaAllocator *idsAllocator, ArenaAllocator *skyLightsAllocator, ArenaAllocator *torchLightsAllocator, ArenaAllocator *indicesAllocator, ArenaAllocator *peeksAllocator) :
  seed(seed),
  chunkDistance(chunkDistance),
  lastCoord(0, 256, 0),
  positionsAllocator(positionsAllocator),
  normalsAllocator(normalsAllocator),
  uvsAllocator(uvsAllocator),
  barycentricsAllocator(barycentricsAllocator),
  aosAllocator(aosAllocator),
  idsAllocator(idsAllocator),
  skyLightsAllocator(skyLightsAllocator),
  torchLightsAllocator(torchLightsAllocator),
  indicesAllocator(indicesAllocator),
  peeksAllocator(peeksAllocator)
  {}
void Tracker::updateNeededCoords(ThreadPool *threadPool, GeometrySet *geometrySet, float x, float y, float z) {
  Coord coord(
    (int)std::floor(x/(float)SUBPARCEL_SIZE),
    (int)std::floor(y/(float)SUBPARCEL_SIZE),
    (int)std::floor(z/(float)SUBPARCEL_SIZE)
  );

  // std::cout << "check coord " << x << " " << y << " " << z << " " << coord.x << " " << coord.y << " " << coord.z << " " << coord.index << " : " << lastCoord.x << " " << lastCoord.y << " " << lastCoord.z << " " << lastCoord.index << " " << (coord != lastCoord) << std::endl;
  if (coord != lastCoord) {
    std::vector<Coord> neededCoords;
    neededCoords.reserve(256);
    std::vector<Coord> addedCoords;
    addedCoords.reserve(256);
    for (int dx = -chunkDistance; dx <= chunkDistance; dx++) {
      const int ax = dx + coord.x;
      for (int dy = -chunkDistance; dy <= chunkDistance; dy++) {
        const int ay = dy + coord.y;
        for (int dz = -chunkDistance; dz <= chunkDistance; dz++) {
          const int az = dz + coord.z;
          Coord aCoord(ax, ay, az);
          neededCoords.push_back(aCoord);

          auto iter = std::find_if(lastNeededCoords.begin(), lastNeededCoords.end(), [&](const Coord &coord2) -> bool {
            return coord2.index == aCoord.index;
          });
          if (iter == lastNeededCoords.end()) {
          	// std::cout << "add index " << coord.x << " " << coord.y << " " << coord.z << " " << aCoord.x << " " << aCoord.y << " " << aCoord.z << " " << aCoord.index << " " << dx << " " << dy << " " << dz << std::endl;
            addedCoords.push_back(aCoord);
          }
        }
      }
    }

    std::vector<Coord> removedCoords;
    removedCoords.reserve(256);
    for (const Coord &lastNeededCoord : lastNeededCoords) {
      auto iter = std::find_if(neededCoords.begin(), neededCoords.end(), [&](const Coord &coord2) -> bool {
        return coord2.index == lastNeededCoord.index;
      });
      if (iter == neededCoords.end()) {
        removedCoords.push_back(lastNeededCoord);
      }
    }

    for (const Coord &addedCoord : addedCoords) {
      Subparcel *subparcel = new Subparcel(addedCoord);
      subparcels[addedCoord.index] = subparcel;
      // std::cout << "added subparcel " << addedCoord.x << " " << addedCoord.y << " " << addedCoord.z << " " << addedCoord.index << " " << addedCoords.size() << " " << subparcels.size() << std::endl;

      unsigned int count = 128;
      Message *message = (Message *)malloc(sizeof(Message) - 4 + count*sizeof(unsigned int));
      message->id = -1;
      message->method = (int)METHODS::chunk;
      message->count = count;

      {
        unsigned int *u32 = (unsigned int *)message->args;
        u32[0] = seed;
        u32[1] = (unsigned int)this;
        u32[2] = (unsigned int)geometrySet;
        u32[3] = (unsigned int)subparcel;
      }

      threadPool->inbox.queue(message);
    }
    for (const Coord &removedCoord : removedCoords) {
      threadPool->inbox.filterQueue([&](Message *message) -> bool {
        if (message->method == (int)METHODS::chunk) {
          unsigned int *u32 = (unsigned int *)message->args;
          Subparcel *subparcel = (Subparcel *)(u32 + 2);
          if (subparcel->coord != removedCoord) {
            return true;
          } else {
          	// std::cout << "dequeue subparcel " << removedCoord.x << " " << removedCoord.y << " " << removedCoord.z << std::endl;
          	return false;
          }
        } else {
          return false;
        }
      });

      subparcels.erase(removedCoord.index); // XXX delete the actual subparcel
    }

    lastNeededCoords = std::move(neededCoords);
    lastCoord = coord;

    // if (addedCoords.size() > 0 || removedCoords.size() > 0) {
      std::cout << "added removed coords " << addedCoords.size() << " : " << removedCoords.size() << std::endl;
    // }
  }
}