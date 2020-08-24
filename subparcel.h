#ifndef SUBPARCEL_H
#define SUBPARCEL_H

#include "vector.h"
#include <deque>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>

constexpr int PARCEL_SIZE = 300;
constexpr int SUBPARCEL_SIZE = 10;
constexpr int SUBPARCEL_SIZE_P1 = SUBPARCEL_SIZE + 1;
constexpr int SUBPARCEL_SIZE_P3 = SUBPARCEL_SIZE + 3;
constexpr int PLANET_OBJECT_SLOTS = 512;
constexpr int MAX_NAME_LENGTH = 32;
constexpr float SPAWNER_RATE = 0.08;
constexpr float slabRadius = 8.660254037844387; // Math.sqrt((SUBPARCEL_SIZE/2)*(SUBPARCEL_SIZE/2)*3);

int absi(int n);
int sign(int n);
int getSubparcelIndex(int x, int y, int z);
unsigned int getFieldIndex(unsigned int x, unsigned int y, unsigned int z);

class Semaphore {
private:
  std::mutex mutex_;
  std::condition_variable condition_;
  unsigned long count_ = 0; // Initialized as locked.
public:
  void release();
  void acquire();
  bool try_acquire();
};

class FreeEntry {
public:
  unsigned int start;
  unsigned int count;
};
class ArenaAllocator {
public:
  ArenaAllocator(unsigned int size);
  FreeEntry *alloc(unsigned int size);
  void free(FreeEntry *freeEntry);
  void updateFreeList();

  unsigned char *data;
  std::vector<FreeEntry *> freeList;
  std::mutex mutex;
};

class Message {
public:
  int id;
  int method;
  unsigned int count;
  unsigned char args[4];
};

template <typename M>
class Mailbox {
public:
  void queue(M *message) {
    std::lock_guard<std::mutex> lock(mutex);
    messages.push_back(message);
    semaphore.release();
  }
  void queueAll(std::vector<M *> &&ms) {
    std::lock_guard<std::mutex> lock(mutex);
    for (M *m : ms) {
      messages.push_back(m);
      semaphore.release();
    }
  }
  M *wait() {
    for(;;) {
      semaphore.acquire();
      std::lock_guard<std::mutex> lock(mutex);
      M *message = messages.front();
      messages.pop_front();
      if (message) {
        return message;
      }
    }
  }
  void push(M *message) {
    std::lock_guard<std::mutex> lock(mutex);
    messages.push_back(message);
    // std::cout << "push messages a " << messages.size() << std::endl;
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
  void popAll(std::vector<M *> &ms) {
    std::lock_guard<std::mutex> lock(mutex);
    // std::cout << "pop all a " << messages.size() << std::endl;
    ms.reserve(messages.size());
    for (Message *message : messages) {
      ms.push_back(message);
    }
    messages.clear();
    // std::cout << "pop all b " << ms.size() << std::endl;
  }
  void filterQueue(std::function<bool(Message *message)> testFn) {
    std::lock_guard<std::mutex> lock(mutex);

    for (auto iter = messages.begin(); iter != messages.end(); iter++) {
      Message *message = *iter;
      if (!testFn(message)) {
        *iter = nullptr;
      }
    }
  }

  std::deque<M *> messages;
  Semaphore semaphore;
  std::mutex mutex;
};

enum class METHODS : int {
  makeArenaAllocator = 0,
  arenaAlloc,
  arenaFree,
  makeGeometrySet,
  loadBake,
  getGeometry,
  getAnimalGeometry,
  marchObjects,
  getHeight,
  noise,
  marchingCubes,
  bakeGeometry,
  chunk,
};
enum class MESSAGES : int {
  updateGeometry = -1,
};
extern "C" {
  extern std::function<void(Message *)> METHOD_FNS[];
}

class ThreadPool {
public:
  ThreadPool(unsigned int numThreads);

  Mailbox<Message> inbox;
  Mailbox<Message> outbox;
};

class Coord {
public:
  Coord() : x(0), y(0), z(0), index(getSubparcelIndex(x, y, z)) {}
  Coord(int x, int y, int z) : x(x), y(y), z(z), index(getSubparcelIndex(x, y, z)) {}
  Coord(int x, int y, int z, int index) : x(x), y(y), z(z), index(index) {}
  Coord(const Coord &coord) : x(coord.x), y(coord.y), z(coord.z), index(coord.index) {}
  bool operator<(const Coord &c) const {
    return c.index < index;
  }
  bool operator==(const Coord &c) const {
    return c.index == index;
  }
  bool operator!=(const Coord &c) const {
    return c.index != index;
  }
  Coord &operator=(const Coord &c) {
    x = c.x;
    y = c.y;
    z = c.z;
    index = c.index;
    return *this;
  }

  int x;
  int y;
  int z;
  int index;
};

class GeometrySet;
class GeometrySpec;
class FreeEntry;
class Subparcel;
namespace physx {
  class PxDefaultAllocator;
  class PxDefaultErrorCallback;
  class PxFoundation;
  class PxPhysics;
  class PxCooking;
}
using namespace physx;
class Tracker {
public:
  Tracker(
    int seed,
    int chunkDistance,

    ArenaAllocator *landPositionsAllocator,
    ArenaAllocator *landNormalsAllocator,
    ArenaAllocator *landUvsAllocator,
    ArenaAllocator *landBarycentricsAllocator,
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
  );
  void updateNeededCoords(ThreadPool *threadPool, GeometrySet *geometrySet, float x, float y, float z);

  int seed;
  int chunkDistance;

  ArenaAllocator *landPositionsAllocator;
  ArenaAllocator *landNormalsAllocator;
  ArenaAllocator *landUvsAllocator;
  ArenaAllocator *landBarycentricsAllocator;
  ArenaAllocator *landAosAllocator;
  ArenaAllocator *landIdsAllocator;
  ArenaAllocator *landSkyLightsAllocator;
  ArenaAllocator *landTorchLightsAllocator;

  ArenaAllocator *vegetationPositionsAllocator;
  ArenaAllocator *vegetationUvsAllocator;
  ArenaAllocator *vegetationIdsAllocator;
  ArenaAllocator *vegetationIndicesAllocator;
  ArenaAllocator *vegetationSkyLightsAllocator;
  ArenaAllocator *vegetationTorchLightsAllocator;

  Coord lastCoord;
  std::vector<Coord> lastNeededCoords;
  std::map<int, std::shared_ptr<Subparcel>> subparcels;

  PxDefaultAllocator *gAllocator = nullptr;
  PxDefaultErrorCallback *gErrorCallback = nullptr;
  PxFoundation *gFoundation = nullptr;
  PxPhysics *physics = nullptr;
  PxCooking *cooking = nullptr;
  std::set<GeometrySpec *> geometrySpecs;
  std::set<GeometrySpec *> staticGeometrySpecs;
  std::vector<std::set<GeometrySpec *> *> geometrySpecSets{
    &staticGeometrySpecs,
    &geometrySpecs,
  };
  std::mutex gPhysicsMutex;
};

class Group {
public:
  unsigned int start;
  unsigned int count;
  unsigned int materialIndex;
};

enum class OBJECT_TYPE : unsigned int {
  VEGETATION = 1,
  PACKAGE = 2,
};
class Object {
public:
  unsigned int id;
  OBJECT_TYPE type;
  char name[MAX_NAME_LENGTH];
  Vec position;
  Quat quaternion;
};

class Subparcel {
public:
  Subparcel(const Coord &coord, Tracker *tracker);
  ~Subparcel();

  bool operator<(const Subparcel &subparcel) const {
    return coord < subparcel.coord;
  }
  bool operator==(const Subparcel &subparcel) const {
    return coord == subparcel.coord;
  }
  bool operator!=(const Subparcel &subparcel) const {
    return coord != subparcel.coord;
  }

  // data
  Coord coord;
  float potentials[SUBPARCEL_SIZE_P3 * SUBPARCEL_SIZE_P3 * SUBPARCEL_SIZE_P3];
  unsigned char biomes[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 3]; // align
  char heightfield[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 1]; // align
  unsigned char lightfield[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 1]; // align
  unsigned int numObjects;
  Object objects[PLANET_OBJECT_SLOTS];

  // state
  Tracker *tracker;
  Sphere boundingSphere;
  unsigned char peeks[16];

  FreeEntry *landPositionsEntry;
  FreeEntry *landNormalsEntry;
  FreeEntry *landUvsEntry;
  FreeEntry *landBarycentricsEntry;
  FreeEntry *landAosEntry;
  FreeEntry *landIdsEntry;
  FreeEntry *landSkyLightsEntry;
  FreeEntry *landTorchLightsEntry;

  FreeEntry *vegetationPositionsEntry;
  FreeEntry *vegetationUvsEntry;
  FreeEntry *vegetationIdsEntry;
  FreeEntry *vegetationIndicesEntry;
  FreeEntry *vegetationSkyLightsEntry;
  FreeEntry *vegetationTorchLightsEntry;

  Group landGroups[2];
  Group vegetationGroups[1];
  GeometrySpec *physxGeometry;
  std::vector<GeometrySpec *> objectPhysxGeometries;
};

#endif