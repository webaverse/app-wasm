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

EMSCRIPTEN_KEEPALIVE PScene *makePhysics() {
  return new PScene();
}
EMSCRIPTEN_KEEPALIVE unsigned int simulatePhysics(PScene *scene, unsigned int *ids, float *positions, float *quaternions, unsigned int numIds, float elapsedTime) {
  return scene->simulate(ids, positions, quaternions, numIds, elapsedTime);
}

EMSCRIPTEN_KEEPALIVE void raycastPhysics(PScene *scene, float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int *hit, float *position, float *normal, float *distance, unsigned int *objectId, unsigned int *faceIndex, Vec *outPosition, Quat *outQuaternion) {
  scene->raycast(origin, direction, meshPosition, meshQuaternion, *hit, position, normal, *distance, *objectId, *faceIndex, *outPosition, *outQuaternion);
}

EMSCRIPTEN_KEEPALIVE void collidePhysics(PScene *scene, float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int *hit, float *direction, unsigned int *grounded) {
  scene->collide(radius, halfHeight, position, quaternion, meshPosition, meshQuaternion, maxIter, *hit, direction, *grounded);
}

EMSCRIPTEN_KEEPALIVE void addBoxGeometryPhysics(PScene *scene, float *position, float *quaternion, float *size, unsigned int id, unsigned int dynamic) {
  scene->addBoxGeometry(position, quaternion, size, id, dynamic);
}

EMSCRIPTEN_KEEPALIVE void cookGeometryPhysics(PScene *scene, float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  scene->cookGeometry(positions, indices, numPositions, numIndices, data, length, writeStream);
}
EMSCRIPTEN_KEEPALIVE void cookConvexGeometryPhysics(PScene *scene, float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  scene->cookConvexGeometry(positions, indices, numPositions, numIndices, data, length, writeStream);
}

EMSCRIPTEN_KEEPALIVE void addGeometryPhysics(PScene *scene, uint8_t *data, unsigned int length, float *position, float *quaternion, unsigned int id, PxDefaultMemoryOutputStream *writeStream) {
  scene->addGeometry(data, length, position, quaternion, id, writeStream);
}
EMSCRIPTEN_KEEPALIVE void addConvexGeometryPhysics(PScene *scene, uint8_t *data, unsigned int length, float *position, float *quaternion, unsigned int id, PxDefaultMemoryOutputStream *writeStream) {
  scene->addConvexGeometry(data, length, position, quaternion, id, writeStream);
}

EMSCRIPTEN_KEEPALIVE void disableGeometryPhysics(PScene *scene, unsigned int id) {
  scene->disableGeometry(id);
}
EMSCRIPTEN_KEEPALIVE void enableGeometryQueriesPhysics(PScene *scene, unsigned int id) {
  scene->enableGeometryQueries(id);
}
EMSCRIPTEN_KEEPALIVE void removeGeometryPhysics(PScene *scene, unsigned int id) {
  scene->removeGeometry(id);
}

}
