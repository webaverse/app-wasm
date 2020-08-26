#ifndef SUBPARCEL_H
#define SUBPARCEL_H

#include "vector.h"
#include "collide.h"
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

class ArenaAllocator;
class FreeEntrySpec {
public:
  unsigned int start;
  unsigned int count;
};
class FreeEntry {
public:
  FreeEntry(const FreeEntrySpec &spec, ArenaAllocator *arenaAllocator);
  ~FreeEntry();

  FreeEntrySpec spec;
  ArenaAllocator *arenaAllocator;
};
class ArenaAllocator {
public:
  ArenaAllocator(unsigned int size);
  std::shared_ptr<FreeEntry> alloc(unsigned int size);
protected:
  void freeSpec(const FreeEntrySpec &spec);
  void updateFreeList();

public:
  unsigned char *data;
  std::vector<FreeEntrySpec> freeList;
  std::mutex mutex;

  friend class FreeEntry;
};

class Message {
public:
  int id;
  int method;
  int priority;
  unsigned int count;
  unsigned char args[4];
};

template <typename M>
class Mailbox {
public:
  void queue(M *message) {
    std::lock_guard<std::mutex> lock(mutex);
    if (message->priority) {
      messages.push_front(message);
    } else {
      messages.push_back(message);
    }
    semaphore.release();
  }
  void queueAll(std::vector<M *> &&ms) {
    std::lock_guard<std::mutex> lock(mutex);
    for (M *m : ms) {
      if (m->priority) {
        messages.push_front(m);
      } else {
        messages.push_back(m);
      }
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
      if (message && !testFn(message)) {
        free(message);
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
  releaseUpdate,
  mine,
  releaseMine,
  light,
  addObject,
  removeObject,
  releaseAddRemoveObject,
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
class FreeEntry;
class Subparcel;
namespace physx {
  class PxDefaultAllocator;
  class PxDefaultErrorCallback;
  class PxFoundation;
  class PxPhysics;
  class PxCooking;
}
class Tracker {
public:
  Tracker(
    int seed,
    unsigned int meshId,
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
  );
  void updateNeededCoords(ThreadPool *threadPool, GeometrySet *geometrySet, float x, float y, float z);

  int seed;
  unsigned int meshId;
  int chunkDistance;

  ArenaAllocator *landPositionsAllocator;
  ArenaAllocator *landNormalsAllocator;
  ArenaAllocator *landUvsAllocator;
  // ArenaAllocator *landBarycentricsAllocator;
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
  std::mutex subparcelsMutex;

  Physicer physicer;
};

class Group {
public:
  Group() : start(0), count(0), materialIndex(0) {}

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
  
  Subparcel *clone() const;
  void copyLand(const Subparcel &subparcel);
  void copyVegetation(const Subparcel &subparcel);

  // data
  Coord coord;
  float potentials[SUBPARCEL_SIZE_P3 * SUBPARCEL_SIZE_P3 * SUBPARCEL_SIZE_P3];
  unsigned char biomes[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 3]; // align
  char heightfield[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 1]; // align
  unsigned char lightfield[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 1]; // align
  unsigned int numObjects;
  Object objects[PLANET_OBJECT_SLOTS];

  // transient state
  Tracker *tracker;
  Sphere boundingSphere;

  // build state
  bool live;

  unsigned char peeks[16];
  std::shared_ptr<FreeEntry> landPositionsEntry;
  std::shared_ptr<FreeEntry> landNormalsEntry;
  std::shared_ptr<FreeEntry> landUvsEntry;
  // std::shared_ptr<FreeEntry> landBarycentricsEntry;
  std::shared_ptr<FreeEntry> landAosEntry;
  std::shared_ptr<FreeEntry> landIdsEntry;
  std::shared_ptr<FreeEntry> landSkyLightsEntry;
  std::shared_ptr<FreeEntry> landTorchLightsEntry;

  std::shared_ptr<FreeEntry> vegetationPositionsEntry;
  std::shared_ptr<FreeEntry> vegetationUvsEntry;
  std::shared_ptr<FreeEntry> vegetationIdsEntry;
  std::shared_ptr<FreeEntry> vegetationIndicesEntry;
  std::shared_ptr<FreeEntry> vegetationSkyLightsEntry;
  std::shared_ptr<FreeEntry> vegetationTorchLightsEntry;

  Group landGroups[2];
  Group vegetationGroups[1];

  std::shared_ptr<PhysicsGeometry> physxGeometry;
  std::vector<std::shared_ptr<PhysicsGeometry>> objectPhysxGeometries;
};

#endif