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
EMSCRIPTEN_KEEPALIVE unsigned int simulatePhysics(PScene *scene, unsigned int *ids, float *positions, float *quaternions, float *scales, unsigned int *bitfields, unsigned int numIds, float elapsedTime, float *velocities) {
  return scene->simulate(ids, positions, quaternions, scales, bitfields, numIds, elapsedTime, velocities);
}

EMSCRIPTEN_KEEPALIVE void raycastPhysics(PScene *scene, float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int *hit, float *position, float *normal, float *distance, unsigned int *objectId, unsigned int *faceIndex, Vec *outPosition, Quat *outQuaternion) {
  scene->raycast(origin, direction, meshPosition, meshQuaternion, *hit, position, normal, *distance, *objectId, *faceIndex, *outPosition, *outQuaternion);
}

EMSCRIPTEN_KEEPALIVE void raycastPhysicsArray(unsigned int rayCount, PScene *scene, float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int *hit, float *position, float *normal, float *distance, unsigned int *objectId, unsigned int *faceIndex, Vec *outPosition, Quat *outQuaternion) {
  
  for (unsigned int i = 0; i < rayCount; i++) {
    
    scene->raycast(origin, direction, meshPosition, meshQuaternion, *hit, position, normal, *distance, *objectId, *faceIndex, *outPosition, *outQuaternion);

    origin += 3;
    direction += 3;
    meshPosition += 3;
    meshQuaternion += 4;
    hit += 1;
    position += 3;
    normal += 3;
    distance += 1;
    objectId += 1;
    faceIndex += 1;
    outPosition += 1;
    outQuaternion += 1;
  }
}

EMSCRIPTEN_KEEPALIVE void collidePhysics(PScene *scene, float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int *hit, float *direction, unsigned int *grounded, unsigned int *id) {
  scene->collide(radius, halfHeight, position, quaternion, meshPosition, meshQuaternion, maxIter, *hit, direction, *grounded, *id);
}

EMSCRIPTEN_KEEPALIVE void addCapsuleGeometryPhysics(PScene *scene, float *position, float *quaternion, float radius, float halfHeight, float *mat, unsigned int id, unsigned int ccdEnabled) {
  scene->addCapsuleGeometry(position, quaternion, radius, halfHeight, mat, id, ccdEnabled);
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

EMSCRIPTEN_KEEPALIVE void addGeometryPhysics(PScene *scene, uint8_t *data, unsigned int length, float *position, float *quaternion, float *scale, unsigned int id, float *mat, PxDefaultMemoryOutputStream *writeStream) {
  scene->addGeometry(data, length, position, quaternion, scale, id, mat, writeStream);
}
EMSCRIPTEN_KEEPALIVE void addConvexGeometryPhysics(PScene *scene, uint8_t *data, unsigned int length, float *position, float *quaternion, float *scale, unsigned int id, PxDefaultMemoryOutputStream *writeStream) {
  scene->addConvexGeometry(data, length, position, quaternion, scale, id, writeStream);
}

EMSCRIPTEN_KEEPALIVE bool getGeometryPhysics(PScene *scene, unsigned int id, float *positions, unsigned int *numPositions, unsigned int *indices, unsigned int *numIndices, float *bounds) {
  return scene->getGeometry(id, positions, *numPositions, indices, *numIndices, bounds);
}
EMSCRIPTEN_KEEPALIVE bool getBoundsPhysics(PScene *scene, unsigned int id, float *bounds) {
  return scene->getBounds(id, bounds);
}

EMSCRIPTEN_KEEPALIVE void disableGeometryPhysics(PScene *scene, unsigned int id) {
  scene->disableGeometry(id);
}
EMSCRIPTEN_KEEPALIVE void enableGeometryQueriesPhysics(PScene *scene, unsigned int id) {
  scene->enableGeometryQueries(id);
}
EMSCRIPTEN_KEEPALIVE void disableGeometryQueriesPhysics(PScene *scene, unsigned int id) {
  scene->disableGeometryQueries(id);
}
EMSCRIPTEN_KEEPALIVE void enableGeometryPhysics(PScene *scene, unsigned int id) {
  scene->enableGeometry(id);
}
EMSCRIPTEN_KEEPALIVE void setMassAndInertiaPhysics(PScene *scene, unsigned int id, float mass, float *inertia) {
  scene->setMassAndInertia(id, mass, inertia);
}
EMSCRIPTEN_KEEPALIVE void setGravityEnabledPhysics(PScene *scene, unsigned int id, bool enabled) {
  scene->setGravityEnabled(id, enabled);
}
EMSCRIPTEN_KEEPALIVE void removeGeometryPhysics(PScene *scene, unsigned int id) {
  scene->removeGeometry(id);
}
EMSCRIPTEN_KEEPALIVE void setTransformPhysics(PScene *scene, unsigned int id, float *position, float *quaternion, float *scale, bool autoWake) {
  scene->setTransform(id, position, quaternion, scale, autoWake);
}
EMSCRIPTEN_KEEPALIVE void getVelocityPhysics(PScene *scene, unsigned int id, float *velocity) {
  scene->getVelocity(id, velocity);
}
EMSCRIPTEN_KEEPALIVE void setVelocityPhysics(PScene *scene, unsigned int id, float *velocity, bool autoWake) {
  scene->setVelocity(id, velocity, autoWake);
}
EMSCRIPTEN_KEEPALIVE void setAngularVelocityPhysics(PScene *scene, unsigned int id, float *velocity, bool autoWake) {
  scene->setAngularVel(id, velocity, autoWake);
}
EMSCRIPTEN_KEEPALIVE void setLinearLockFlagsPhysics(PScene *scene, unsigned int id, bool x, bool y, bool z) {
  scene->setLinearLockFlags(id, x, y, z);
}
EMSCRIPTEN_KEEPALIVE void setAngularLockFlagsPhysics(PScene *scene, unsigned int id, bool x, bool y, bool z) {
  scene->setAngularLockFlags(id, x, y, z);
}
EMSCRIPTEN_KEEPALIVE PxController *createCharacterControllerPhysics(PScene *scene, float radius, float height, float contactOffset, float stepOffset, float *position, float *mat) {
  return scene->createCharacterController(radius, height, contactOffset, stepOffset, position, mat);
}
EMSCRIPTEN_KEEPALIVE void destroyCharacterControllerPhysics(PScene *scene, PxController *characterController) {
  scene->destroyCharacterController(characterController);
}
EMSCRIPTEN_KEEPALIVE unsigned int moveCharacterControllerPhysics(PScene *scene, PxController *characterController, float *displacement, float minDist, float elapsedTime, float *positionOut) {
  return scene->moveCharacterController(characterController, displacement, minDist, elapsedTime, positionOut);
}
EMSCRIPTEN_KEEPALIVE void setCharacterControllerPositionPhysics(PScene *scene, PxController *characterController, float *position) {
  return scene->setCharacterControllerPosition(characterController, position);
}

}
