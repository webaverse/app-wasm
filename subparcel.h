#ifndef SUBPARCEL_H
#define SUBPARCEL_H

#include "vector.h"

constexpr int SUBPARCEL_SIZE = 10;
constexpr int SUBPARCEL_SIZE_P1 = SUBPARCEL_SIZE + 1;
constexpr int SUBPARCEL_SIZE_P3 = SUBPARCEL_SIZE + 3;
constexpr int PLANET_OBJECT_SLOTS = 512;
constexpr int MAX_NAME_LENGTH = 32;
constexpr float SPAWNER_RATE = 0.08;

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
  int x;
  int y;
  int z;
  float potentials[SUBPARCEL_SIZE_P3 * SUBPARCEL_SIZE_P3 * SUBPARCEL_SIZE_P3];
  unsigned char biomes[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 3]; // align
  char heightfield[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 1]; // align
  unsigned char lightfield[SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1 + 1]; // align
  unsigned int numObjects;
  Object objects[PLANET_OBJECT_SLOTS];
};

#endif