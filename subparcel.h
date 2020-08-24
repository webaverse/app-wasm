#ifndef SUBPARCEL_H
#define SUBPARCEL_H

#include "vector.h"

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
  Group groups[3];
  uintptr_t physxGeometry;
};

#endif