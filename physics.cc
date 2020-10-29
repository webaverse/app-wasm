// #include "DirectXMath.h"
/* #include "PhysX/physx/include/geometry/PxMeshQuery.h"
#include "geometry/PxCapsuleGeometry.h"
#include "foundation/PxTransform.h"
 */
#include "physics.h"

using namespace physx;

PScene::PScene() {
  allocator = new PxDefaultAllocator();
  errorCallback = new PxDefaultErrorCallback();
  foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, *errorCallback);
  PxTolerancesScale tolerancesScale;
  physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, tolerancesScale);
  
  physx::PxSceneDesc sceneDesc = physx::PxSceneDesc((physx::PxTolerancesScale()));
  sceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
  if(!sceneDesc.cpuDispatcher)
  {
      physx::PxDefaultCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(4);
      if(!mCpuDispatcher)
          std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;
      sceneDesc.cpuDispatcher    = mCpuDispatcher;
  }
  if(!sceneDesc.filterShader)
      sceneDesc.filterShader    = &physx::PxDefaultSimulationFilterShader;


  scene = physics->createScene(sceneDesc);
  
  // if(geometry.isValid())
  {
      PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
      PxTransform transform(PxVec3(0, -10, 0));
      PxCapsuleGeometry geometry(1, 1);
      PxRigidDynamic *capsule = PxCreateDynamic(*physics, transform, geometry, *material, 1);
      capsule->userData = (void *)0x1;
      scene->addActor(*capsule);
  }
  {
      PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
      PxTransform transform(PxVec3(0, -10, 0));
      PxBoxGeometry geometry(10, 10, 10);
      PxRigidStatic *floor = PxCreateStatic(*physics, transform, geometry, *material);
      floor->userData = (void *)0x2;
      scene->addActor(*floor);
  }
  {
      PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
      PxTransform transform(PxVec3(0, -10, 0));
      PxBoxGeometry geometry(0.5, 0.5, 0.5);
      PxRigidDynamic *box = PxCreateDynamic(*physics, transform, geometry, *material, 1);
      box->userData = (void *)0x3;
      scene->addActor(*box);
  }
}
PScene::~PScene() {}

void PScene::simulate(unsigned int *ids, float *positions, float *quaternion, float elapsedTime) {
  scene->simulate(elapsedTime);
  PxU32 error;
  scene->fetchResults(true, &error);
  if (error) {
    std::cout << "simulate error " << error << std::endl;
  }
  
  PxU32 numActors;
  PxRigidActor **transforms = (PxRigidActor **)scene->getActiveActors(numActors);
  for (unsigned int i = 0; i < numActors; i++) {
    PxTransform actor2World = transforms[i]->getGlobalPose();
    PxVec3 &p = actor2World.p;
    PxQuat &q = actor2World.q;
  }
}

void PScene::addGeometry(int type, float *position, float *quaternion) {
  
}