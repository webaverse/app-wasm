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
  // tolerancesScale.length = 0.01;
  {
    // PxTolerancesScale tolerancesScale;
    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, tolerancesScale);
  }
  {
    PxCookingParams cookingParams(tolerancesScale);
    cookingParams.meshWeldTolerance = 0.15;
    // cookingParams.planeTolerance = 0;
    // cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
    // cookingParams.meshSizePerformanceTradeOff = 0;
    cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, cookingParams);
  }
  {
    // PxTolerancesScale tolerancesScale;
    PxSceneDesc sceneDesc = PxSceneDesc(tolerancesScale);
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
    if(!sceneDesc.cpuDispatcher)
    {
        physx::PxDefaultCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(0);
        if(!mCpuDispatcher)
            std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;
        sceneDesc.cpuDispatcher    = mCpuDispatcher;
    }
    if(!sceneDesc.filterShader)
        sceneDesc.filterShader    = &physx::PxDefaultSimulationFilterShader;
    sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

    scene = physics->createScene(sceneDesc);
  }
 
  /* {
      PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
      PxTransform transform(PxVec3(0, -10, 0));
      PxCapsuleGeometry geometry(1, 1);
      PxRigidDynamic *capsule = PxCreateDynamic(*physics, transform, geometry, *material, 1);
      capsule->userData = (void *)0x1;
      // capsule->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
      PxRigidBodyExt::updateMassAndInertia(*capsule, 1.0f);
      scene->addActor(*capsule);
      actors.push_back(capsule);
  } */
  /* {
      PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
      PxTransform transform(PxVec3(0, -1, 0));
      PxBoxGeometry geometry(30, 1, 30);
      PxRigidStatic *floor = PxCreateStatic(*physics, transform, geometry, *material);
      floor->userData = (void *)0x2;
      scene->addActor(*floor);
      actors.push_back(floor);
  }
  {
      PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
      PxTransform transform(PxVec3(0, 5, 0));
      PxBoxGeometry geometry(0.5, 0.5, 0.5);
      PxRigidDynamic *box = PxCreateDynamic(*physics, transform, geometry, *material, 1);
      box->userData = (void *)0x3;
      PxRigidBodyExt::updateMassAndInertia(*box, 1.0f);
      scene->addActor(*box);
      actors.push_back(box);
  } */
}
PScene::~PScene() {
  std::cout << "scene destructor" << std::endl;
  abort();
}

unsigned int PScene::simulate(unsigned int *ids, float *positions, float *quaternions, unsigned int numIds, float elapsedTime) {
  for (unsigned int i = 0; i < numIds; i++) {
    unsigned int id = ids[i];
    PxTransform transform(PxVec3(positions[i*3], positions[i*3+1], positions[i*3+2]), PxQuat(quaternions[i*4], quaternions[i*4+1], quaternions[i*4+2], quaternions[i*4+3]));
    auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
      return (unsigned int)actor->userData == id;
    });
    if (actorIter != actors.end()) {
      PxRigidActor *actor = *actorIter;
      actor->setGlobalPose(transform, true);
      // actor->wakeUp();
    } else {
      std::cerr << "unknown actor id " << id << std::endl;
    }
  }

  scene->simulate(elapsedTime);
  PxU32 error;
  scene->fetchResults(true, &error);
  if (error) {
    std::cout << "scene simulate error " << error << std::endl;
  }
  
  PxU32 numActors;
  PxRigidActor **actors = (PxRigidActor **)scene->getActiveActors(numActors);
  for (unsigned int i = 0; i < numActors; i++) {
    const PxTransform &actor2World = actors[i]->getGlobalPose();
    const PxVec3 &p = actor2World.p;
    const PxQuat &q = actor2World.q;
    
    ids[i] = (unsigned int)actors[i]->userData;
    
    positions[i*3] = p.x;
    positions[i*3+1] = p.y;
    positions[i*3+2] = p.z;
    
    quaternions[i*4] = q.x;
    quaternions[i*4+1] = q.y;
    quaternions[i*4+2] = q.z;
    quaternions[i*4+3] = q.w;
  }
  return numActors;
}

void PScene::addBoxGeometry(float *position, float *quaternion, float *size, unsigned int id, unsigned int dynamic) {
  if (dynamic) {
    PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
    PxTransform transform(PxVec3(position[0], position[1], position[2]), PxQuat(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
    PxBoxGeometry geometry(size[0], size[1], size[2]);
    PxRigidDynamic *box = PxCreateDynamic(*physics, transform, geometry, *material, 1);
    box->userData = (void *)id;
    PxRigidBodyExt::updateMassAndInertia(*box, 1.0f);
    scene->addActor(*box);
    actors.push_back(box);
  } else {
    PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
    PxTransform transform(PxVec3(position[0], position[1], position[2]), PxQuat(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
    PxBoxGeometry geometry(size[0], size[1], size[2]);
    PxRigidStatic *floor = PxCreateStatic(*physics, transform, geometry, *material);
    floor->userData = (void *)id;
    scene->addActor(*floor);
    actors.push_back(floor);
  }
}
void PScene::cookGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  PxVec3 *verts = (PxVec3 *)positions;
  PxU32 nbVerts = numPositions/3;
  PxU32 *indices32 = (PxU32 *)indices;
  PxU32 triCount = numIndices/3;

  PxTriangleMeshDesc meshDesc{};
  meshDesc.points.count           = nbVerts;
  meshDesc.points.stride          = sizeof(PxVec3);
  meshDesc.points.data            = verts;

  meshDesc.triangles.count        = triCount;
  meshDesc.triangles.stride       = 3*sizeof(PxU32);
  meshDesc.triangles.data         = indices32;

  /* bool ok = cooking->validateTriangleMesh(meshDesc);
  if (!ok) {
    std::cerr << "invalid triangle mesh" << std::endl;
  } */

  *writeStream = new PxDefaultMemoryOutputStream();
  bool status = cooking->cookTriangleMesh(meshDesc, **writeStream);
  if (!status) {
    std::cerr << "geometry triangle mesh bake failed" << std::endl;
  }

  *data = (*writeStream)->getData();
  *length = (*writeStream)->getSize();
}
void PScene::cookConvexGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
  PxVec3 *verts = (PxVec3 *)positions;
  PxU32 nbVerts = numPositions/3;
  PxU32 *indices32 = (PxU32 *)indices;
  PxU32 triCount = numIndices/3;

  PxConvexMeshDesc meshDesc{};
  meshDesc.points.count           = nbVerts;
  meshDesc.points.stride          = sizeof(PxVec3);
  meshDesc.points.data            = verts;

  meshDesc.indices.count        = triCount;
  meshDesc.indices.stride       = 3*sizeof(PxU32);
  meshDesc.indices.data         = indices32;

  meshDesc.flags            = PxConvexFlag::eCOMPUTE_CONVEX;
  // meshDesc.maxVerts         = 10;
  
  *writeStream = new PxDefaultMemoryOutputStream();
  bool status = cooking->cookConvexMesh(meshDesc, **writeStream);
  if (!status) {
    std::cerr << "geometry convex mesh bake failed" << std::endl;
  }

  *data = (*writeStream)->getData();
  *length = (*writeStream)->getSize();
}
void PScene::addGeometry(uint8_t *data, unsigned int length, PxDefaultMemoryOutputStream *writeStream) {
  PxDefaultMemoryInputData readBuffer(data, length);
  PxTriangleMesh *triangleMesh = physics->createTriangleMesh(readBuffer);

  PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
  PxTransform transform(PxVec3(0, 0, 0));
  PxTriangleMeshGeometry geometry(triangleMesh);
  PxRigidStatic *mesh = PxCreateStatic(*physics, transform, geometry, *material);
  mesh->userData = (void *)0x4;
  scene->addActor(*mesh);
  actors.push_back(mesh);

  if (writeStream) {
    delete writeStream;
  }
}
void PScene::removeGeometry(unsigned int id) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    actor->release();
    actors.erase(actorIter);
  } else {
    std::cerr << "unknown actor id " << id << std::endl;
  }
}

/* std::shared_ptr<PhysicsObject> doMakeGeometry(Physicer *physicer, PxGeometry *geometry, unsigned int objectId, float *meshPosition, float *meshQuaternion) {
  Vec p(meshPosition[0], meshPosition[1], meshPosition[2]);
  Quat q(meshQuaternion[0], meshQuaternion[1], meshQuaternion[2], meshQuaternion[3]);
  std::shared_ptr<PhysicsGeometry> geometrySpec(new PhysicsGeometry(nullptr, nullptr, geometry));
  std::shared_ptr<PhysicsObject> geometryObject(new PhysicsObject(objectId, p, q, std::move(geometrySpec), p, q, physicer));
  return std::move(geometryObject);
} */

void PScene::raycast(float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int &hit, float *position, float *normal, float &distance, unsigned int &objectId, unsigned int &faceIndex, Vec &outPosition, Quat &outQuaternion) {
  PxVec3 originVec{origin[0], origin[1], origin[2]};
  PxVec3 directionVec{direction[0], direction[1], direction[2]};
  Ray ray(Vec{origin[0], origin[1], origin[2]}, Vec{direction[0], direction[1], direction[2]});
  PxTransform meshPose(
    PxVec3{meshPosition[0], meshPosition[1], meshPosition[2]},
    PxQuat{meshQuaternion[0], meshQuaternion[1], meshQuaternion[2], meshQuaternion[3]}
  );
  Vec p(meshPosition[0], meshPosition[1], meshPosition[2]);
  Quat q(meshQuaternion[0], meshQuaternion[1], meshQuaternion[2], meshQuaternion[3]);

  PxRaycastHit hitInfo;
  constexpr float maxDist = 1000.0;
  const PxHitFlags hitFlags = PxHitFlag::eDEFAULT;
  constexpr PxU32 maxHits = 1;

  {
    hit = 0;
    for (unsigned int i = 0; i < actors.size(); i++) {
      PxRigidActor *actor = actors[i];
      PxShape *shape;
      actor->getShapes(&shape, 1);

      PxGeometryHolder holder = shape->getGeometry();
      PxGeometry &geometry = holder.any();
      PxTransform meshPose2 = actor->getGlobalPose();
      PxTransform meshPose3 = meshPose * meshPose2;
      // PxTransform meshPose4 = meshPose2 * meshPose;

      PxU32 hitCount = PxGeometryQuery::raycast(originVec, directionVec,
                                                geometry,
                                                meshPose3,
                                                maxDist,
                                                hitFlags,
                                                maxHits, &hitInfo);

      if (hitCount > 0 && (!hit || hitInfo.distance < distance)) {
        hit = 1;
        position[0] = hitInfo.position.x;
        position[1] = hitInfo.position.y;
        position[2] = hitInfo.position.z;
        normal[0] = hitInfo.normal.x;
        normal[1] = hitInfo.normal.y;
        normal[2] = hitInfo.normal.z;
        distance = hitInfo.distance;
        objectId = (unsigned int)actor->userData;
        outPosition = Vec(meshPose2.p.x, meshPose2.p.y, meshPose2.p.z);
        outQuaternion = Quat(meshPose2.q.x, meshPose2.q.y, meshPose2.q.z, meshPose2.q.w);
        faceIndex = hitInfo.faceIndex;
      }
    }
  }
}

void PScene::collide(float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int &hit, float *direction, unsigned int &grounded) {
  PxCapsuleGeometry geom(radius, halfHeight);
  PxTransform geomPose(
    PxVec3{position[0], position[1], position[2]},
    PxQuat{quaternion[0], quaternion[1], quaternion[2], quaternion[3]}
  );
  PxTransform meshPose{
    PxVec3{meshPosition[0], meshPosition[1], meshPosition[2]},
    PxQuat{meshQuaternion[0], meshQuaternion[1], meshQuaternion[2], meshQuaternion[3]}
  };

  Vec p(meshPosition[0], meshPosition[1], meshPosition[2]);
  Quat q(meshQuaternion[0], meshQuaternion[1], meshQuaternion[2], meshQuaternion[3]);
  
  Vec offset(0, 0, 0);
  bool anyHadHit = false;
  bool anyHadGrounded = false;
  {
    for (unsigned int i = 0; i < maxIter; i++) {
      bool hadHit = false;
      // for (const std::tuple<bool, std::shared_ptr<PhysicsObject>> &t : sortedGeometrySpecs) {
      for (unsigned int i = 0; i < actors.size(); i++) {
        PxRigidActor *actor = actors[i];
        PxShape *shape;
        actor->getShapes(&shape, 1);

        PxGeometryHolder holder = shape->getGeometry();
        PxGeometry &geometry = holder.any();

        PxTransform meshPose2 = actor->getGlobalPose();
        PxTransform meshPose3 = meshPose * meshPose2;

        PxVec3 directionVec;
        PxReal depthFloat;
        bool result = PxGeometryQuery::computePenetration(directionVec, depthFloat, geom, geomPose, geometry, meshPose3);
        if (result) {
          anyHadHit = true;
          hadHit = true;
          offset += Vec(directionVec.x, directionVec.y, directionVec.z)*depthFloat;
          geomPose.p.x += directionVec.x*depthFloat;
          geomPose.p.y += directionVec.y*depthFloat;
          geomPose.p.z += directionVec.z*depthFloat;
          anyHadGrounded = anyHadGrounded || directionVec.y > 0;
          // break;
        }
      }
      if (hadHit) {
        continue;
      } else {
        break;
      }
    }
  }
  if (anyHadHit) {
    hit = 1;
    direction[0] = offset.x;
    direction[1] = offset.y;
    direction[2] = offset.z;
    grounded = +anyHadGrounded;
  } else {
    hit = 0;
  }
}