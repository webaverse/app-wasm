#ifndef SUBPARCEL_H
#define SUBPARCEL_H

#include "vector.h"
#include <deque>
#include <map>
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

  int x;
  int y;
  int z;
  int index;
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
  Subparcel() {
    initState();
  }
  Subparcel(int x, int y, int z) : coord(x, y, z) {
    initState();
  }
  Subparcel(int x, int y, int z, int index) : coord(x, y, z, index) {
    initState();
  }
  Subparcel(const Coord &coord) : coord(coord) {
    initState();
  }
  inline void initState() {
    boundingSphere = Sphere(
      Vec{(float)coord.x*(float)SUBPARCEL_SIZE + (float)SUBPARCEL_SIZE/2.0f, (float)coord.y*(float)SUBPARCEL_SIZE + (float)SUBPARCEL_SIZE/2.0f, (float)coord.z*(float)SUBPARCEL_SIZE + (float)SUBPARCEL_SIZE/2.0f},
      slabRadius
    );
  }

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
  Sphere boundingSphere;
  unsigned char peeks[16];
  Group landGroups[2];
  Group vegetationGroups[1];
  uintptr_t physxGeometry;
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

class GeometrySet;
class Tracker {
public:
  Tracker(int seed, int chunkDistance, ArenaAllocator *positionsAllocator, ArenaAllocator *normalsAllocator, ArenaAllocator *uvsAllocator, ArenaAllocator *barycentricsAllocator, ArenaAllocator *aosAllocator, ArenaAllocator *idsAllocator, ArenaAllocator *skyLightsAllocator, ArenaAllocator *torchLightsAllocator, ArenaAllocator *indicesAllocator, ArenaAllocator *peeksAllocator);
  void updateNeededCoords(ThreadPool *threadPool, GeometrySet *geometrySet, float x, float y, float z);

  int seed;
  int chunkDistance;
  ArenaAllocator *positionsAllocator;
  ArenaAllocator *normalsAllocator;
  ArenaAllocator *uvsAllocator;
  ArenaAllocator *barycentricsAllocator;
  ArenaAllocator *aosAllocator;
  ArenaAllocator *idsAllocator;
  ArenaAllocator *skyLightsAllocator;
  ArenaAllocator *torchLightsAllocator;
  ArenaAllocator *indicesAllocator;
  ArenaAllocator *peeksAllocator;
  Coord lastCoord;
  std::vector<Coord> lastNeededCoords;
  std::map<int, Subparcel *> subparcels;
};

#endif