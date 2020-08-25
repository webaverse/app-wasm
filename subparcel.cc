#include "subparcel.h"
#include "collide.h"
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
		std::vector<FreeEntry *> freeList2;
		freeList2.reserve(freeList.size());
		std::copy_if(freeList.begin(), freeList.end(), std::back_inserter(freeList2), [](FreeEntry *freeEntry) -> bool {
      return freeEntry != nullptr;
		});
		freeList = std::move(freeList2);
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
        } else {
        	free(message);
        }

        // std::cout << "push message " << outbox << " " << outbox.messages.size() << std::endl;
      }
    });
    thread.detach();
  }
}

Tracker::Tracker(
	int seed,
  int chunkDistance,

  ArenaAllocator *landPositionsAllocator,
  ArenaAllocator *landNormalsAllocator,
  ArenaAllocator *landUvsAllocator,
  // ArenaAllocator *landBarycentricsAllocator,
  ArenaAllocator *landAosAllocator,
  ArenaAllocator *landIdsAllocator,
  ArenaAllocator *landSkyLightsAllocator,
  ArenaAllocator *landTorchLightsAllocator,

  ArenaAllocator *vegetationPositionsAllocator,
  ArenaAllocator *vegetationUvsAllocator,
  ArenaAllocator *vegetationIdsAllocator,
  ArenaAllocator *vegetationIndicesAllocator,
  ArenaAllocator *vegetationSkyLightsAllocator,
  ArenaAllocator *vegetationTorchLightsAllocator
) :
  seed(seed),
  chunkDistance(chunkDistance),
  lastCoord(0, 256, 0),

  landPositionsAllocator(landPositionsAllocator),
  landNormalsAllocator(landNormalsAllocator),
  landUvsAllocator(landUvsAllocator),
  // landBarycentricsAllocator(landBarycentricsAllocator),
  landAosAllocator(landAosAllocator),
  landIdsAllocator(landIdsAllocator),
  landSkyLightsAllocator(landSkyLightsAllocator),
  landTorchLightsAllocator(landTorchLightsAllocator),

  vegetationPositionsAllocator(vegetationPositionsAllocator),
  vegetationUvsAllocator(vegetationUvsAllocator),
  vegetationIdsAllocator(vegetationIdsAllocator),
  vegetationIndicesAllocator(vegetationIndicesAllocator),
  vegetationSkyLightsAllocator(vegetationSkyLightsAllocator),
  vegetationTorchLightsAllocator(vegetationTorchLightsAllocator)
{
  gAllocator = new PxDefaultAllocator();
  gErrorCallback = new PxDefaultErrorCallback();
  gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *gAllocator, *gErrorCallback);
  PxTolerancesScale tolerancesScale;
  physics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, tolerancesScale);
  PxCookingParams cookingParams(tolerancesScale);
  // cookingParams.midphaseDesc = PxMeshMidPhase::eBVH34;
  cooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, cookingParams);
}
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
      std::shared_ptr<Subparcel> subparcel(new Subparcel(addedCoord, this));
      subparcels[addedCoord.index] = subparcel;

      unsigned int count = 128;
      Message *message = (Message *)malloc(sizeof(Message) - 4 + count*sizeof(unsigned int));
      message->id = -1;
      message->method = (int)METHODS::chunk;
      message->priority = 0;
      message->count = count;

      {
        unsigned int *u32 = (unsigned int *)message->args;
        u32[0] = seed;
        u32[1] = (unsigned int)this;
        u32[2] = (unsigned int)geometrySet;
        u32[3] = (unsigned int)(new std::shared_ptr<Subparcel>(subparcel));
      }

      threadPool->inbox.queue(message);
    }
    for (const Coord &removedCoord : removedCoords) {
      threadPool->inbox.filterQueue([&](Message *message) -> bool {
        if (message->method == (int)METHODS::chunk) {
          unsigned int *u32 = (unsigned int *)message->args;
          std::shared_ptr<Subparcel> *subparcel = *((std::shared_ptr<Subparcel> **)(u32 + 3));
          if ((*subparcel)->coord != removedCoord) {
          	return true;
          } else {
          	delete subparcel;
          	return false;
          }
        } else {
          return true;
        }
      });

      subparcels.erase(removedCoord.index);
    }

    lastNeededCoords = std::move(neededCoords);
    lastCoord = coord;

    // if (addedCoords.size() > 0 || removedCoords.size() > 0) {
      std::cout << "added removed coords " << addedCoords.size() << " : " << removedCoords.size() << std::endl;
    // }
  }
}

Subparcel::Subparcel(const Coord &coord, Tracker *tracker) :
  coord(coord),
  tracker(tracker),
  boundingSphere(
    Vec{(float)coord.x*(float)SUBPARCEL_SIZE + (float)SUBPARCEL_SIZE/2.0f, (float)coord.y*(float)SUBPARCEL_SIZE + (float)SUBPARCEL_SIZE/2.0f, (float)coord.z*(float)SUBPARCEL_SIZE + (float)SUBPARCEL_SIZE/2.0f},
    slabRadius
  ),

  landPositionsEntry(nullptr),
  landNormalsEntry(nullptr),
  landUvsEntry(nullptr),
  // landBarycentricsEntry(nullptr),
  landAosEntry(nullptr),
  landIdsEntry(nullptr),
  landSkyLightsEntry(nullptr),
  landTorchLightsEntry(nullptr),

  vegetationPositionsEntry(nullptr),
  vegetationUvsEntry(nullptr),
  vegetationIdsEntry(nullptr),
  vegetationIndicesEntry(nullptr),
  vegetationSkyLightsEntry(nullptr),
  vegetationTorchLightsEntry(nullptr),

  physxGeometry(nullptr)
{}
Subparcel::~Subparcel() {
  // std::cout << "delete subparcel " << coord.x << " " << coord.y << " " << coord.z << std::endl;

  if (landPositionsEntry) tracker->landPositionsAllocator->free(landPositionsEntry);
  if (landNormalsEntry) tracker->landNormalsAllocator->free(landNormalsEntry);
  if (landUvsEntry) tracker->landUvsAllocator->free(landUvsEntry);
  // if (landBarycentricsEntry) tracker->landBarycentricsAllocator->free(landBarycentricsEntry);
  if (landAosEntry) tracker->landAosAllocator->free(landAosEntry);
  if (landIdsEntry) tracker->landIdsAllocator->free(landIdsEntry);
  if (landSkyLightsEntry) tracker->landSkyLightsAllocator->free(landSkyLightsEntry);
  if (landTorchLightsEntry) tracker->landTorchLightsAllocator->free(landTorchLightsEntry);

  if (vegetationPositionsEntry) tracker->vegetationPositionsAllocator->free(vegetationPositionsEntry);
  if (vegetationUvsEntry) tracker->vegetationUvsAllocator->free(vegetationUvsEntry);
  if (vegetationIdsEntry) tracker->vegetationIdsAllocator->free(vegetationIdsEntry);
  if (vegetationIndicesEntry) tracker->vegetationIndicesAllocator->free(vegetationIndicesEntry);
  if (vegetationSkyLightsEntry) tracker->vegetationSkyLightsAllocator->free(vegetationSkyLightsEntry);
  if (vegetationTorchLightsEntry) tracker->vegetationTorchLightsAllocator->free(vegetationTorchLightsEntry);

  {
    std::lock_guard<std::mutex> lock(tracker->gPhysicsMutex);

    if (physxGeometry) {
      for (std::set<GeometrySpec *> *geometrySpecSet : tracker->geometrySpecSets) {
        geometrySpecSet->erase(physxGeometry);
      }
    }
    for (GeometrySpec *geometrySpec : objectPhysxGeometries) {
      for (std::set<GeometrySpec *> *geometrySpecSet : tracker->geometrySpecSets) {
        geometrySpecSet->erase(geometrySpec);
      }
    }
  }
}