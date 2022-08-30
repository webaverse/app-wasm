#include <emscripten.h>
// #include "geometry.h"
// #include "compose.h"
// #include "noise.h"
// #include "march.h"
// #include "collide.h"
#include "physics.h"
// #include "convex.h"
// #include "earcut.h"
// #include <iostream>
// #include "cut.h"
#include <deque>
#include <map>

extern "C" {

// memory

EMSCRIPTEN_KEEPALIVE void *doMalloc(size_t size) {
  return malloc(size);
}

EMSCRIPTEN_KEEPALIVE void doFree(void *ptr) {
  free(ptr);
}

//

EMSCRIPTEN_KEEPALIVE void initialize() {
  physicsBase = new PBase();
}

//

EMSCRIPTEN_KEEPALIVE PBase *makePhysicsBase() {
  return new PBase();
}

EMSCRIPTEN_KEEPALIVE void cookGeometryPhysics(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  physicsBase->cookGeometry(positions, indices, numPositions, numIndices, data, length, writeStream);
}
EMSCRIPTEN_KEEPALIVE void cookConvexGeometryPhysics(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  physicsBase->cookConvexGeometry(positions, indices, numPositions, numIndices, data, length, writeStream);
}
EMSCRIPTEN_KEEPALIVE void cookHeightFieldGeometryPhysics(unsigned int width, unsigned int height, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  physicsBase->cookHeightFieldGeometry(width, height, data, length, writeStream);
}

EMSCRIPTEN_KEEPALIVE void deleteMemoryOutputStream(PxDefaultMemoryOutputStream *writeStream) {
  delete writeStream;
}

} // extern "C"