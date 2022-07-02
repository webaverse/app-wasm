#include <emscripten.h>
// #include "geometry.h"
// #include "compose.h"
// #include "noise.h"
#include "march.h"
#include "DualContouring/main.h"
#include "AnimationSystem/AnimationSystem.h"
// #include "collide.h"
#include "physics.h"
// #include "convex.h"
// #include "earcut.h"
// #include <iostream>
#include "cut.h"
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
EMSCRIPTEN_KEEPALIVE PxD6Joint *addJointPhysics(PScene *scene, unsigned int id1, unsigned int id2, float *position1, float *position2, float *quaternion1, float *quaternion2) {
  return scene->addJoint(id1, id2, position1, position2, quaternion1, quaternion2);
}
EMSCRIPTEN_KEEPALIVE void setJointMotionPhysics(PScene *scene, PxD6Joint *joint, PxD6Axis::Enum axis, PxD6Motion::Enum motion) {
  return scene->setJointMotion(joint, axis, motion);
}
EMSCRIPTEN_KEEPALIVE void setJointTwistLimitPhysics(PScene *scene, PxD6Joint *joint, float lowerLimit, float upperLimit, float contactDist = -1.0f) {
  return scene->setJointTwistLimit(joint, lowerLimit, upperLimit, contactDist);
}
EMSCRIPTEN_KEEPALIVE void setJointSwingLimitPhysics(PScene *scene, PxD6Joint *joint, float yLimitAngle, float zLimitAngle, float contactDist = -1.0f) {
  return scene->setJointSwingLimit(joint, yLimitAngle, zLimitAngle, contactDist);
}
EMSCRIPTEN_KEEPALIVE bool updateMassAndInertiaPhysics(PScene *scene, unsigned int id, float shapeDensities) {
  return scene->updateMassAndInertia(id, shapeDensities);
}
EMSCRIPTEN_KEEPALIVE float getBodyMassPhysics(PScene *scene, unsigned int id) {
  return scene->getBodyMass(id);
}

// AnimationSystem

EMSCRIPTEN_KEEPALIVE float setTest(float num) {
  return AnimationSystem::setTest(num);
}
EMSCRIPTEN_KEEPALIVE float setTest2(float num) {
  return AnimationSystem::setTest2(num);
}
EMSCRIPTEN_KEEPALIVE float setTestNew(float num) {
  return AnimationSystem::setTestNew(num);
}
EMSCRIPTEN_KEEPALIVE float setTestAlloc(float num) {
  return AnimationSystem::setTestAlloc(num);
}
EMSCRIPTEN_KEEPALIVE float getTest() {
  return AnimationSystem::getTest();
}
// ------
// todo: use pointer instead of index.
// EMSCRIPTEN_KEEPALIVE AnimationMixer *createAnimationMixer(unsigned int avatarId) {
EMSCRIPTEN_KEEPALIVE void createAnimationMixer(unsigned int avatarId) {
  return AnimationSystem::createAnimationMixer(avatarId);
}
EMSCRIPTEN_KEEPALIVE float **updateAnimationMixer(float timeS, float f) {
  return AnimationSystem::updateAnimationMixer(timeS, f);
}
EMSCRIPTEN_KEEPALIVE void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone, bool isTop, bool isArm) {
  return AnimationSystem::createAnimationMapping(isPosition, index, isFirstBone, isLastBone, isTop, isArm);
}
EMSCRIPTEN_KEEPALIVE AnimationSystem::Animation *createAnimation(float duration) {
  return AnimationSystem::createAnimation(duration);
}
EMSCRIPTEN_KEEPALIVE AnimationSystem::Animation *getAnimation1(unsigned int index) {
  return AnimationSystem::getAnimation1(index);
}
EMSCRIPTEN_KEEPALIVE float *getAnimation2(unsigned int index) {
  return AnimationSystem::getAnimation2(index);
}
EMSCRIPTEN_KEEPALIVE unsigned int *getAnimation3(unsigned int index) {
  return AnimationSystem::getAnimation3(index);
}
EMSCRIPTEN_KEEPALIVE float **getAnimation4(unsigned int index) {
  return AnimationSystem::getAnimation4(index);
}
EMSCRIPTEN_KEEPALIVE unsigned int **getAnimation5(unsigned int index) {
  return AnimationSystem::getAnimation5(index);
}
EMSCRIPTEN_KEEPALIVE AnimationSystem::Animation **getAnimation6(unsigned int index) {
  return AnimationSystem::getAnimation6(index);
}
EMSCRIPTEN_KEEPALIVE AnimationSystem::Animation **getAnimation7(unsigned int index) {
  return AnimationSystem::getAnimation7(index);
}
EMSCRIPTEN_KEEPALIVE float getFloat1() {
  return AnimationSystem::getFloat1();
}
EMSCRIPTEN_KEEPALIVE float *getFloat2() {
  return AnimationSystem::getFloat2();
}
EMSCRIPTEN_KEEPALIVE float **getFloat3() {
  return AnimationSystem::getFloat3();
}
// EMSCRIPTEN_KEEPALIVE AnimationSystem::Animation getAnimationB1() { // objectize.cc:110:49: warning: 'getAnimationB1' has C-linkage specified, but returns user-defined type 'AnimationSystem::Animation' which is incompatible with C [-Wreturn-type-c-linkage]
//   return AnimationSystem::getAnimationB1();
// }
EMSCRIPTEN_KEEPALIVE AnimationSystem::Animation *getAnimationB2() {
  return AnimationSystem::getAnimationB2();
}
EMSCRIPTEN_KEEPALIVE AnimationSystem::Animation **getAnimationB3() {
  return AnimationSystem::getAnimationB3();
}
EMSCRIPTEN_KEEPALIVE float **getAnimationB4() {
  return AnimationSystem::getAnimationB4();
}
EMSCRIPTEN_KEEPALIVE unsigned int **getAnimationB5() {
  return AnimationSystem::getAnimationB5();
}
EMSCRIPTEN_KEEPALIVE AnimationSystem::AnimationNode *createMotion(AnimationSystem::Animation *animation) {
  return AnimationSystem::createMotion(animation);
}
EMSCRIPTEN_KEEPALIVE AnimationSystem::AnimationNode *createNode(AnimationSystem::NodeType type = AnimationSystem::NodeType::LIST) {
  return AnimationSystem::createNode(type);
}
EMSCRIPTEN_KEEPALIVE void addChild(AnimationSystem::AnimationNode *parent, AnimationSystem::AnimationNode *child) {
  return AnimationSystem::addChild(parent, child);
}
EMSCRIPTEN_KEEPALIVE void setAnimTree(AnimationSystem::AnimationNode *node) {
  return AnimationSystem::setAnimTree(node);
}
EMSCRIPTEN_KEEPALIVE void createInterpolant(unsigned int animationIndex, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize) {
  return AnimationSystem::createInterpolant(animationIndex, numParameterPositions, parameterPositions, numSampleValues, sampleValues, valueSize);
}
EMSCRIPTEN_KEEPALIVE float *evaluateInterpolant(unsigned int animationIndex, unsigned int interpolantIndex, float t) {
  return AnimationSystem::evaluateInterpolant(animationIndex, interpolantIndex, t);
}
EMSCRIPTEN_KEEPALIVE float **getAnimationValues(unsigned int animationIndex, float t) {
  return AnimationSystem::getAnimationValues(animationIndex, t);
}
EMSCRIPTEN_KEEPALIVE void crossFadeTwo(AnimationSystem::AnimationNode *parentNode, float duration, float targetFactor) {
  // return AnimationSystem::crossFadeTwo(parentNode, duration, targetFactor);
  return parentNode->crossFadeTwo(duration, targetFactor);
}
EMSCRIPTEN_KEEPALIVE void crossFadeUnitary(AnimationSystem::AnimationNode *parentNode, float duration, AnimationSystem::AnimationNode *targetNode) {
  // return AnimationSystem::crossFadeUnitary(parentNode, duration, targetNode);
  return parentNode->crossFadeUnitary(duration, targetNode);
}
EMSCRIPTEN_KEEPALIVE void setWeight(AnimationSystem::AnimationNode *node, float weight) {
  return AnimationSystem::setWeight(node, weight); // todo: run `node->weight = weight;` directly.
}
EMSCRIPTEN_KEEPALIVE void setFactor(AnimationSystem::AnimationNode *node, float factor) {
  return AnimationSystem::setFactor(node, factor);
}
EMSCRIPTEN_KEEPALIVE float getWeight(AnimationSystem::AnimationNode *node) {
  return AnimationSystem::getWeight(node);
}
EMSCRIPTEN_KEEPALIVE float getFactor(AnimationSystem::AnimationNode *node) {
  return AnimationSystem::getFactor(node);
}
EMSCRIPTEN_KEEPALIVE void play(AnimationSystem::AnimationNode *motion) {
  return motion->play();
}
EMSCRIPTEN_KEEPALIVE void stop(AnimationSystem::AnimationNode *motion) {
  return motion->stop();
}
EMSCRIPTEN_KEEPALIVE void setTimeBias(AnimationSystem::AnimationNode *motion, float timeBias) {
  return motion->setTimeBias(timeBias);
}
EMSCRIPTEN_KEEPALIVE void setSpeed(AnimationSystem::AnimationNode *motion, float speed) {
  return motion->setSpeed(speed);
}
EMSCRIPTEN_KEEPALIVE void setLoop(AnimationSystem::AnimationNode *motion, AnimationSystem::LoopType loopType) {
  return motion->setLoop(loopType);
}

// End AnimationSystem

EMSCRIPTEN_KEEPALIVE unsigned int simulatePhysics(PScene *scene, unsigned int *ids, float *positions, float *quaternions, float *scales, unsigned int *bitfields, unsigned int numIds, float elapsedTime, float *velocities) {
  return scene->simulate(ids, positions, quaternions, scales, bitfields, numIds, elapsedTime, velocities);
}

EMSCRIPTEN_KEEPALIVE void raycastPhysics(PScene *scene, float *origin, float *direction, float maxDist, unsigned int *hit, float *position, float *normal, float *distance, unsigned int *objectId, unsigned int *faceIndex) {
  scene->raycast(origin, direction, maxDist, *hit, position, normal, *distance, *objectId, *faceIndex);
}

EMSCRIPTEN_KEEPALIVE void raycastPhysicsArray(unsigned int rayCount, PScene *scene, float *origin, float *direction, float maxDist, unsigned int *hit, float *position, float *normal, float *distance, unsigned int *objectId, unsigned int *faceIndex) {
  for (unsigned int i = 0; i < rayCount; i++) {
    scene->raycast(origin, direction, maxDist, *hit, position, normal, *distance, *objectId, *faceIndex);

    origin += 3;
    direction += 3;
    hit += 1;
    position += 3;
    normal += 3;
    distance += 1;
    objectId += 1;
    faceIndex += 1;
  }
}

EMSCRIPTEN_KEEPALIVE void sweepBox(
  PScene *scene,
  float *origin,
  float *quaternion,
  float *halfExtents,
  float *direction,
  float sweepDistance,
  unsigned int maxHits,
  unsigned int *numHits,
  float *position,
  float *normal,
  float *distance,
  unsigned int *objectId,
  unsigned int *faceIndex
) {
  scene->sweepBox(
    origin,
    quaternion,
    halfExtents,
    direction,
    sweepDistance,
    maxHits,
    *numHits,
    position,
    normal,
    distance,
    objectId,
    faceIndex
  );
}
EMSCRIPTEN_KEEPALIVE void sweepConvexShape(
  PScene *scene,
  PxConvexMesh *convexMesh,
  float *origin,
  float *quaternion,
  float *direction,
  float sweepDistance,
  unsigned int maxHits,
  unsigned int *numHits,
  float *position,
  float *normal,
  float *distance,
  unsigned int *objectId,
  unsigned int *faceIndex
) {
  scene->sweepConvexShape(
    convexMesh,
    origin,
    quaternion,
    direction,
    sweepDistance,
    maxHits,
    *numHits,
    position,
    normal,
    distance,
    objectId,
    faceIndex
  );
}
EMSCRIPTEN_KEEPALIVE float *getPathPhysics(PScene *scene, float *_start, float *_dest, bool _isWalk, float _hy, float _heightTolerance, unsigned int _maxIterdetect, unsigned int _maxIterStep, unsigned int _numIgnorePhysicsIds, unsigned int *_ignorePhysicsIds) {
  return scene->getPath(_start, _dest, _isWalk, _hy, _heightTolerance, _maxIterdetect, _maxIterStep, _numIgnorePhysicsIds, _ignorePhysicsIds);
}

EMSCRIPTEN_KEEPALIVE float *overlapBoxPhysics(PScene *scene, float hx, float hy, float hz, float *position, float *quaternion, float *meshPosition, float *meshQuaternion) {
  return scene->overlapBox(hx, hy, hz, position, quaternion, meshPosition, meshQuaternion);
}
EMSCRIPTEN_KEEPALIVE float *overlapCapsulePhysics(PScene *scene, float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion) {
  return scene->overlapCapsule(radius, halfHeight, position, quaternion, meshPosition, meshQuaternion);
}
EMSCRIPTEN_KEEPALIVE void collideBoxPhysics(PScene *scene, float hx, float hy, float hz, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int *hit, float *direction, unsigned int *grounded, unsigned int *id) {
  scene->collideBox(hx, hy, hz, position, quaternion, meshPosition, meshQuaternion, maxIter, *hit, direction, *grounded, *id);
}
EMSCRIPTEN_KEEPALIVE void collideCapsulePhysics(PScene *scene, float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int *hit, float *direction, unsigned int *grounded, unsigned int *id) {
  scene->collideCapsule(radius, halfHeight, position, quaternion, meshPosition, meshQuaternion, maxIter, *hit, direction, *grounded, *id);
}
EMSCRIPTEN_KEEPALIVE void getCollisionObjectPhysics(PScene *scene, float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int *hit, unsigned int *id) {
  scene->getCollisionObject(radius, halfHeight, position, quaternion, meshPosition, meshQuaternion, *hit, *id);
}
EMSCRIPTEN_KEEPALIVE void addCapsuleGeometryPhysics(PScene *scene, float *position, float *quaternion, float radius, float halfHeight, unsigned int id, PxMaterial *material, unsigned int dynamic, unsigned int flags) {
  scene->addCapsuleGeometry(position, quaternion, radius, halfHeight, id, material, dynamic, flags);
}

EMSCRIPTEN_KEEPALIVE void addBoxGeometryPhysics(PScene *scene, float *position, float *quaternion, float *size, unsigned int id, PxMaterial *material, unsigned int dynamic, int groupId) {
  scene->addBoxGeometry(position, quaternion, size, id, material, dynamic, groupId);
}

EMSCRIPTEN_KEEPALIVE void cookGeometryPhysics(PScene *scene, float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  scene->cookGeometry(positions, indices, numPositions, numIndices, data, length, writeStream);
}
EMSCRIPTEN_KEEPALIVE void cookConvexGeometryPhysics(PScene *scene, float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  scene->cookConvexGeometry(positions, indices, numPositions, numIndices, data, length, writeStream);
}

EMSCRIPTEN_KEEPALIVE PxTriangleMesh *createShapePhysics(PScene *scene, uint8_t *data, unsigned int length, PxDefaultMemoryOutputStream *releaseWriteStream) {
  return scene->createShape(data, length, releaseWriteStream);
}
EMSCRIPTEN_KEEPALIVE void destroyShapePhysics(PScene *scene, PxTriangleMesh *triangleMesh) {
  scene->destroyShape(triangleMesh);
}

EMSCRIPTEN_KEEPALIVE PxConvexMesh *createConvexShapePhysics(PScene *scene, uint8_t *data, unsigned int length, PxDefaultMemoryOutputStream *releaseWriteStream) {
  return scene->createConvexShape(data, length, releaseWriteStream);
}
EMSCRIPTEN_KEEPALIVE void destroyConvexShapePhysics(PScene *scene, PxConvexMesh *convexMesh) {
  scene->destroyConvexShape(convexMesh);
}

EMSCRIPTEN_KEEPALIVE PxMaterial *createMaterialPhysics(PScene *scene, float *mat) {
  return scene->createMaterial(mat);
}
EMSCRIPTEN_KEEPALIVE void destroyMaterialPhysics(PScene *scene, PxMaterial *material) {
  scene->destroyMaterial(material);
}

EMSCRIPTEN_KEEPALIVE void addGeometryPhysics(PScene *scene, PxTriangleMesh *triangleMesh, float *position, float *quaternion, float *scale, unsigned int id, PxMaterial *material, unsigned int external, PxTriangleMesh *releaseTriangleMesh) {
  scene->addGeometry(triangleMesh, position, quaternion, scale, id, material, external, releaseTriangleMesh);
}
EMSCRIPTEN_KEEPALIVE void addConvexGeometryPhysics(PScene *scene, PxConvexMesh *convexMesh, float *position, float *quaternion, float *scale, unsigned int id, PxMaterial *material, unsigned int dynamic, unsigned int external, PxConvexMesh *releaseConvexMesh) {
  scene->addConvexGeometry(convexMesh, position, quaternion, scale, id, material, dynamic, external, releaseConvexMesh);
}

EMSCRIPTEN_KEEPALIVE void setGeometryScalePhysics(PScene *scene, unsigned int id, float *scale, PxDefaultMemoryOutputStream *writeStream) {
  scene->setGeometryScale(id, scale, writeStream);
}

EMSCRIPTEN_KEEPALIVE bool getGeometryPhysics(PScene *scene, unsigned int id, float *positions, unsigned int *numPositions, unsigned int *indices, unsigned int *numIndices, float *bounds) {
  return scene->getGeometry(id, positions, *numPositions, indices, *numIndices, bounds);
}
EMSCRIPTEN_KEEPALIVE bool getBoundsPhysics(PScene *scene, unsigned int id, float *bounds) {
  return scene->getBounds(id, bounds);
}

EMSCRIPTEN_KEEPALIVE void enableActorPhysics(PScene *scene, unsigned int id) {
  scene->enableActor(id);
}
EMSCRIPTEN_KEEPALIVE void disableActorPhysics(PScene *scene, unsigned int id) {
  scene->disableActor(id);
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
EMSCRIPTEN_KEEPALIVE void getGlobalPositionPhysics(PScene *scene, unsigned int id, float *position) {
  scene->getGlobalPosition(id, position);
}
EMSCRIPTEN_KEEPALIVE void getLinearVelocityPhysics(PScene *scene, unsigned int id, float *velocity) {
  scene->getLinearVelocity(id, velocity);
}
EMSCRIPTEN_KEEPALIVE void getAngularVelocityPhysics(PScene *scene, unsigned int id, float *velocity) {
  scene->getAngularVelocity(id, velocity);
}
EMSCRIPTEN_KEEPALIVE void addForceAtPosPhysics(PScene *scene, unsigned int id, float *velocity, float *position, bool autoWake) {
  scene->addForceAtPos(id, velocity, position, autoWake);
}
EMSCRIPTEN_KEEPALIVE void addForceAtLocalPosPhysics(PScene *scene, unsigned int id, float *velocity, float *position, bool autoWake) {
  scene->addForceAtLocalPos(id, velocity, position, autoWake);
}
EMSCRIPTEN_KEEPALIVE void addLocalForceAtPosPhysics(PScene *scene, unsigned int id, float *velocity, float *position, bool autoWake) {
  scene->addLocalForceAtPos(id, velocity, position, autoWake);
}
EMSCRIPTEN_KEEPALIVE void addLocalForceAtLocalPosPhysics(PScene *scene, unsigned int id, float *velocity, float *position, bool autoWake) {
  scene->addLocalForceAtLocalPos(id, velocity, position, autoWake);
}
EMSCRIPTEN_KEEPALIVE void addForcePhysics(PScene *scene, unsigned int id, float *velocity, bool autoWake) {
  scene->addForce(id, velocity, autoWake);
}
EMSCRIPTEN_KEEPALIVE void addTorquePhysics(PScene *scene, unsigned int id, float *velocity, bool autoWake) {
  scene->addTorque(id, velocity, autoWake);
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
EMSCRIPTEN_KEEPALIVE PxController *createCharacterControllerPhysics(PScene *scene, float radius, float height, float contactOffset, float stepOffset, float *position, PxMaterial *material, unsigned int id) {
  return scene->createCharacterController(radius, height, contactOffset, stepOffset, position, material, id);
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

EMSCRIPTEN_KEEPALIVE float *doCut(
  float *positions,
  unsigned int numPositions,
  float *normals,
  unsigned int numNormals,
  float *uvs,
  unsigned int numUvs,
  unsigned int *faces,
  unsigned int numFaces,

  float *planeNormal,
  float planeDistance
) {
  return cut(
    positions,
    numPositions,
    normals,
    numNormals,
    uvs,
    numUvs,
    faces,
    numFaces,

    planeNormal,
    planeDistance
  );
}

// EMSCRIPTEN_KEEPALIVE void doMarchingingCubes(
//   int dims[3],
//   float *potential,
//   uint8_t *brush,
//   float shift[3],
//   float scale[3],
//   float *positions,
//   float *colors,
//   unsigned int *faces,
//   unsigned int *positionIndex,
//   unsigned int *colorIndex,
//   unsigned int *faceIndex)
// {
//   marchingCubes(dims, potential, brush, shift, scale, positions, colors, faces, *positionIndex, *colorIndex, *faceIndex);
// }

EMSCRIPTEN_KEEPALIVE uint8_t *doMarchingCubes(int dims[3], float *potential, float shift[3], float scale[3]) {
  return marchingCubes(dims, potential, shift, scale);
}


EMSCRIPTEN_KEEPALIVE void generateChunkDataDualContouring(float x, float y, float z){
    return DualContouring::generateChunkData(x, y, z);
}

EMSCRIPTEN_KEEPALIVE void setChunkLodDualContouring(float x, float y, float z, int lod){
    return DualContouring::setChunkLod(x, y, z, lod);
}

EMSCRIPTEN_KEEPALIVE void clearTemporaryChunkDataDualContouring(){
    return DualContouring::clearTemporaryChunkData();
}

EMSCRIPTEN_KEEPALIVE void clearChunkRootDualContouring(float x, float y, float z){
    return DualContouring::clearChunkRoot(x, y, z);
}

EMSCRIPTEN_KEEPALIVE uint8_t *createChunkMeshDualContouring(float x, float y, float z){
    return DualContouring::createChunkMesh(x, y, z);
}

/* EMSCRIPTEN_KEEPALIVE bool drawDamage(float x, float y, float z, float radius, float value) {
    return DualContouring::drawDamage(x, y, z, radius, value);
} */

} // extern "C"