#ifndef _PHYSICS_H
#define _PHYSICS_H

#include "vector.h"
#include "march.h"
#include "PxPhysicsVersion.h"
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxSceneDesc.h"
#include "PxRigidStatic.h"
#include "PxRigidDynamic.h"
#include "PxMaterial.h"
#include "extensions/PxDefaultStreams.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "extensions/PxDefaultCpuDispatcher.h"
#include "extensions/PxDefaultSimulationFilterShader.h"
#include "extensions/PxRevoluteJoint.h"
#include "extensions/PxSphericalJoint.h"
#include "extensions/PxD6Joint.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "cooking/PxTriangleMeshDesc.h"
#include "cooking/PxCooking.h"
#include "extensions/PxTriangleMeshExt.h"
#include "extensions/PxSimpleFactory.h"
#include "extensions/PxRigidBodyExt.h"
#include "extensions/PxRigidActorExt.h"
// #include "PxPhysicsAPI.h"
#include "PxQueryReport.h"
#include "PxSimulationEventCallback.h"
#include "characterkinematic/PxControllerManager.h"
#include "characterkinematic/PxCapsuleController.h"
#include "geometry/PxGeometryQuery.h"
#include <list>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include <iostream>

using namespace physx;

enum TYPE {
  TYPE_NONE = 0,
  TYPE_CAPSULE = 1,
};
enum STATE_BITFIELD {
  STATE_BITFIELD_NONE = 0x0,
  STATE_BITFIELD_COLLIDED = 0x1,
  STATE_BITFIELD_GROUNDED = 0x2,
};

class SimulationEventCallback2 : public PxSimulationEventCallback {
public:
  std::map<unsigned int, unsigned int> stateBitfields;

  SimulationEventCallback2();
  virtual ~SimulationEventCallback2();
  virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count);
  virtual void onWake(PxActor **actors, PxU32 count);
  virtual void onSleep(PxActor **actors, PxU32 count);
  virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
  virtual void onTrigger(PxTriggerPair *pairs, PxU32 count);
  virtual void onAdvance(const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer, const PxU32 count);
};

class CharacterControllerFilterCallback : public PxQueryFilterCallback {
public:
  CharacterControllerFilterCallback();
  virtual ~CharacterControllerFilterCallback();
  virtual PxQueryHitType::Enum preFilter(const PxFilterData &filterData, const PxShape *shape, const PxRigidActor *actor, PxHitFlags &queryFlags);
  virtual PxQueryHitType::Enum postFilter(const PxFilterData &filterData, const PxQueryHit &hit);
};

class Bone {
public:
  uint32_t id;
  std::string name;
  PxVec3 position;
  PxQuat quaternion;
  PxVec3 scale;
  float radius;
  float halfHeight;
  float boneLength;
  std::vector<std::unique_ptr<Bone>> children;
  PxRigidBody *body;
  // PxRigidActor *body;
  PxJoint *joint;
};

class PScene {
public:
  PScene();
  ~PScene();

  unsigned int simulate(unsigned int *ids, float *positions, float *quaternions, float *scales, unsigned int *bitfields, unsigned int numIds, float elapsedTime, float *velocities);
  void raycast(float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int &hit, float *position, float *normal, float &distance, unsigned int &objectId, unsigned int &faceIndex, Vec &outPosition, Quat &outQuaternion);
  void collide(PxGeometry *geom, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int &hit, float *direction, unsigned int &grounded, unsigned int &id);
  void collideBox(float hx, float hy, float hz, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int &hit, float *direction, unsigned int &grounded, unsigned int &id);
  void collideCapsule(float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int &hit, float *direction, unsigned int &grounded, unsigned int &id);
  void getCollisionObject(float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int &hit, unsigned int &id);
  void addCapsuleGeometry(float *position, float *quaternion, float radius, float halfHeight, float *mat, unsigned int id, unsigned int flags);
  void addBoxGeometry(float *position, float *quaternion, float *size, unsigned int id, unsigned int dynamic);
  void cookGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream);
  void cookConvexGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream);
  void addGeometry(uint8_t *data, unsigned int length, float *position, float *quaternion, float *scale, unsigned int id, float *mat, PxDefaultMemoryOutputStream *writeStream);
  void addConvexGeometry(uint8_t *data, unsigned int length, float *position, float *quaternion, float *scale, unsigned int id, PxDefaultMemoryOutputStream *writeStream);
  void setMassAndInertia(unsigned int id, float mass, float *inertia);
  void setGravityEnabled(unsigned int id, bool enabled);
  void removeGeometry(unsigned int id);
  bool getGeometry(unsigned int id, float *positions, unsigned int &numPositions, unsigned int *indices, unsigned int &numIndices, float *bounds);
  bool getBounds(unsigned int id, float *bounds);
  void disableGeometry(unsigned int id);
  void enableGeometry(unsigned int id);
  void disableGeometryQueries(unsigned int id);
  void enableGeometryQueries(unsigned int id);
  void setTransform(unsigned int id, float *position, float *quaternion, float *scale, bool autoWake);
  void getGlobalPosition(unsigned int id, float *position);
  void getVelocity(unsigned int id, float *velocity);
  void setVelocity(unsigned int id, float *velocity, bool autoWake);
  void setAngularVel(unsigned int id, float *velocity, bool autoWake);
  void setLinearLockFlags(unsigned int id, bool x, bool y, bool z);
  void setAngularLockFlags(unsigned int id, bool x, bool y, bool z);
  PxController *createCharacterController(float radius, float height, float contactOffset, float stepOffset, float *position, float *mat, unsigned int groupId);
  void destroyCharacterController(PxController *characterController);
  unsigned int moveCharacterController(PxController *characterController, float *displacement, float minDist, float elapsedTime, float *positionOut);
  void setCharacterControllerPosition(PxController *characterController, float *position);
  void registerSkeleton(Bone &bone, Bone *parentBone, unsigned int groupId);
  Bone *createSkeleton(unsigned char *buffer, unsigned int groupId);
  void setSkeletonFromBuffer(Bone *skeleton, unsigned char *buffer);

  PxDefaultAllocator *allocator = nullptr;
  PxDefaultErrorCallback *errorCallback = nullptr;
  PxFoundation *foundation = nullptr;
  PxPhysics *physics = nullptr;
  PxCooking *cooking = nullptr;
  PxScene *scene = nullptr;
  PxControllerManager *controllerManager = nullptr;
  std::vector<PxRigidActor *> actors;
  SimulationEventCallback2 *simulationEventCallback = nullptr;
};

#endif