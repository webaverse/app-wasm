// #include "DirectXMath.h"
/* #include "PhysX/physx/include/geometry/PxMeshQuery.h"
#include "geometry/PxCapsuleGeometry.h"
#include "foundation/PxTransform.h"
 */
#include "physics.h"

using namespace physx;

SimulationEventCallback2::SimulationEventCallback2() {}
SimulationEventCallback2::~SimulationEventCallback2() {}
void SimulationEventCallback2::onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}
void SimulationEventCallback2::onWake(PxActor **actors, PxU32 count) {}
void SimulationEventCallback2::onSleep(PxActor **actors, PxU32 count) {}
void SimulationEventCallback2::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
  PxRigidActor *actor1 = pairHeader.actors[0];
  PxRigidActor *actor2 = pairHeader.actors[1];

  unsigned int actor1Id = (unsigned int)actor1->userData;
  unsigned int actor2Id = (unsigned int)actor2->userData;

  stateBitfields[actor1Id] |= STATE_BITFIELD::STATE_BITFIELD_COLLIDED;
  stateBitfields[actor2Id] |= STATE_BITFIELD::STATE_BITFIELD_COLLIDED;

  for (uint32_t i = 0; i < nbPairs; i++) {
    const PxContactPair &pair = pairs[i];

    PxContactPairPoint contactPairPoints[32];
    uint32_t numPoints = pair.extractContacts(contactPairPoints, sizeof(contactPairPoints) / sizeof(contactPairPoints[0]));
    
    for (uint32_t j = 0; j < numPoints; j++) {
      PxContactPairPoint &contactPairPoint = contactPairPoints[j];
      if (contactPairPoint.normal.y >= 0.0001) { // from B to A is up
        stateBitfields[actor1Id] |= STATE_BITFIELD::STATE_BITFIELD_GROUNDED;
      }
      if (contactPairPoint.normal.y <= -0.0001) { // from B to A is down
        stateBitfields[actor2Id] |= STATE_BITFIELD::STATE_BITFIELD_GROUNDED;
      }
    }
  }

  /* PxContactPairPoint contactPoints[32];
  auto numPoints = pairs->extractContacts(contactPoints, sizeof(contactPoints)/sizeof(contactPoints[0]));
  for (auto i = 0; i < numPoints; i++) {
    const PxContactPairPoint &contactPint = contactPoints[i];
  } */

  // std::cerr << "on contact" << std::endl;
}
void SimulationEventCallback2::onTrigger(PxTriggerPair *pairs, PxU32 count) {}
void SimulationEventCallback2::onAdvance(const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer, const PxU32 count) {}

PxFilterFlags ccdFilterShader(
  PxFilterObjectAttributes attributes0,
  PxFilterData filterData0,
  PxFilterObjectAttributes attributes1,
  PxFilterData filterData1,
  PxPairFlags& pairFlags,
  const void* constantBlock,
  PxU32 constantBlockSize
) {
  PxFilterFlags result = physx::PxDefaultSimulationFilterShader(
    attributes0,
    filterData0,
    attributes1,
    filterData1,
    pairFlags,
    constantBlock,
    constantBlockSize
  );
  pairFlags |= PxPairFlag::eSOLVE_CONTACT;
  pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
  pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
  /* if (filterData0.word1 == TYPE::TYPE_CAPSULE || filterData1.word1 == TYPE::TYPE_CAPSULE) {
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
    pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
  } */
  return result;
}

enum PhysicsObjectFlags {
  NONE = 0,
  ENABLE_PHYSICS = 1,
  ENABLE_CCD = 2,
};

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
    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
    // cookingParams.meshSizePerformanceTradeOff = 0;
    cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, cookingParams);
  }
  {
    simulationEventCallback = new SimulationEventCallback2();
  }
  {
    // PxTolerancesScale tolerancesScale;
    PxSceneDesc sceneDesc = PxSceneDesc(tolerancesScale);
    sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
    sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    sceneDesc.broadPhaseType = PxBroadPhaseType::eABP;
    sceneDesc.simulationEventCallback = simulationEventCallback;
    if (!sceneDesc.cpuDispatcher) {
      physx::PxDefaultCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(0);
      if (!mCpuDispatcher) {
        std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;
      }
      sceneDesc.cpuDispatcher = mCpuDispatcher;
    }
    sceneDesc.filterShader = ccdFilterShader;
    scene = physics->createScene(sceneDesc);
    controllerManager = PxCreateControllerManager(*scene);
    controllerManager->setOverlapRecoveryModule(true);
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

unsigned int PScene::simulate(unsigned int *ids, float *positions, float *quaternions, float *scales, unsigned int *stateBitfields, unsigned int numIds, float elapsedTime, float *velocities) {
  for (unsigned int i = 0; i < numIds; i++) {
    unsigned int id = ids[i];
    //PxTransform transform(PxVec3(positions[i*3], positions[i*3+1], positions[i*3+2]), PxQuat(quaternions[i*4], quaternions[i*4+1], quaternions[i*4+2], quaternions[i*4+3]));
    auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
      return (unsigned int)actor->userData == id;
    });
    if (actorIter != actors.end()) {
      PxRigidActor *actor = *actorIter;
      //actor->setGlobalPose(transform, true);
      PxRigidBody *body = dynamic_cast<PxRigidBody *>(actor);
      if (body) {
        // std::cout << "reset" << std::endl;
        body->setLinearVelocity(PxVec3(velocities[i*3], velocities[i*3+1], velocities[i*3+2]), true);
        //body->setAngularVelocity(PxVec3(0, 0, 0), true);
        // actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
      }
      // actor->wakeUp();
    } else {
      std::cerr << "simulate unknown actor id " << id << std::endl;
    }
  }

  simulationEventCallback->stateBitfields.clear();

  scene->simulate(elapsedTime);
  PxU32 error;
  scene->fetchResults(true, &error);
  if (error) {
    std::cout << "scene simulate error " << error << std::endl;
  }
  
  PxU32 numActors;
  PxRigidActor **actors = (PxRigidActor **)scene->getActiveActors(numActors);
  for (unsigned int i = 0; i < numActors; i++) {
    PxRigidActor *actor = actors[i];
    const PxTransform &actor2World = actor->getGlobalPose();
    const PxVec3 &p = actor2World.p;
    const PxQuat &q = actor2World.q;
    
    const unsigned int id = (unsigned int)actors[i]->userData;
    ids[i] = id;
    
    positions[i*3] = p.x;
    positions[i*3+1] = p.y;
    positions[i*3+2] = p.z;
    
    quaternions[i*4] = q.x;
    quaternions[i*4+1] = q.y;
    quaternions[i*4+2] = q.z;
    quaternions[i*4+3] = q.w;
    
    {
      PxVec3 s(1, 1, 1);
      unsigned int numShapes = actor->getNbShapes();
      if (numShapes == 1) {
        PxShape *shapes[1];
        actor->getShapes(shapes, sizeof(shapes)/sizeof(shapes[0]), 0);
        PxShape *shape = shapes[0];
        PxGeometryHolder geometryHolder = shape->getGeometry();
        PxGeometryType::Enum geometryType = geometryHolder.getType();
        switch (geometryType) {
          case PxGeometryType::Enum::eBOX: {
            const PxBoxGeometry &geometry = geometryHolder.box();
            const PxVec3 &halfExtents = geometry.halfExtents;
            s = halfExtents * 2;
            break;
          }
          case PxGeometryType::Enum::eCONVEXMESH: {
            PxConvexMeshGeometry &geometry = geometryHolder.convexMesh();
            s = geometry.scale.scale;
            break;
          }
          case PxGeometryType::Enum::eTRIANGLEMESH: {
            PxTriangleMeshGeometry &geometry = geometryHolder.triangleMesh();
            s = geometry.scale.scale;
            break;
          }
          case PxGeometryType::Enum::eCAPSULE: {
            PxCapsuleGeometry &geometry = geometryHolder.capsule();
            s = PxVec3(geometry.radius * 2.0);
            break;
          }
          default: {
            std::cerr << "unknown geometry type for actor id " << id << " : " << (unsigned int)geometryType << std::endl;
            break;
          }
        }
      } else {
        std::cerr << "no shapes for actor id " << id << " : " << numShapes << std::endl;
      }
      
      scales[i*3] = s.x;
      scales[i*3+1] = s.y;
      scales[i*3+2] = s.z;

      // std::cout << "check bitfields 1" << std::endl;
      stateBitfields[i] = simulationEventCallback->stateBitfields[id];
      // std::cout << "check bitfields 2" << std::endl;
    }
  }
  return numActors;
}

void PScene::addCapsuleGeometry(
  float *position,
  float *quaternion,
  float radius,
  float halfHeight,
  float *mat,
  unsigned int id,
  unsigned int flags
) {
  // PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
  PxMaterial *material = physics->createMaterial(mat[0], mat[1], mat[2]);
  PxTransform transform(PxVec3(position[0], position[1], position[2]), PxQuat(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
  PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0,0,1)));
  PxCapsuleGeometry geometry(radius, halfHeight);
  PxRigidDynamic *body = PxCreateDynamic(*physics, transform, geometry, *material, 1);

  // flags
  const bool physicsEnabled = (bool)(flags & PhysicsObjectFlags::ENABLE_PHYSICS);
  /* if (!physicsEnabled) {
    PxRigidActor *actor = body;

    constexpr int numShapes = 32;
    PxShape *shapes[numShapes];
    for (int j = 0; ; j++) {
      memset(shapes, 0, sizeof(shapes));
      if (actor->getShapes(shapes, numShapes, j * numShapes) == 0) {
        break;
      }
      for (int i = 0; i < numShapes; ++i) {
        if (shapes[i] == nullptr) {
          break;
        }

        PxShape *rigidShape = shapes[i];
        rigidShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
      }
    }
  } */
  const bool ccdEnabled = (bool)(flags & PhysicsObjectFlags::ENABLE_CCD);
  if (ccdEnabled) {
    body->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
  }

  /* {
    // LOL 1
    PxFilterData filterData{};
    filterData.word0 = id;
    filterData.word1 = TYPE::TYPE_CAPSULE;
    // filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a
                                    // contact callback;
    const PxU32 numShapes = body->getNbShapes();
    PxShape *shapes[32];
    body->getShapes(shapes, numShapes);
    for (PxU32 i = 0; i < numShapes; i++){
      PxShape *shape = shapes[i];
      shape->setSimulationFilterData(filterData);
    }
    // LOL 2
  } */

  body->userData = (void *)id;

  PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);
  if (physicsEnabled) {
    scene->addActor(*body);
  }
  actors.push_back(body);
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

void PScene::addGeometry(uint8_t *data, unsigned int length, float *position, float *quaternion, float *scale, unsigned int id, float *mat, PxDefaultMemoryOutputStream *writeStream) {
  PxDefaultMemoryInputData readBuffer(data, length);
  PxTriangleMesh *triangleMesh = physics->createTriangleMesh(readBuffer);

  // PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
  PxMaterial *material = physics->createMaterial(mat[0], mat[1], mat[2]);
  PxTransform transform(PxVec3(position[0], position[1], position[2]), PxQuat(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
  PxMeshScale scaleObject(PxVec3(scale[0], scale[1], scale[2]));
  PxTriangleMeshGeometry geometry(triangleMesh, scaleObject);
  PxRigidStatic *mesh = PxCreateStatic(*physics, transform, geometry, *material);
  mesh->userData = (void *)id;
  scene->addActor(*mesh);
  actors.push_back(mesh);

  if (writeStream) {
    delete writeStream;
  }
}
void PScene::addConvexGeometry(uint8_t *data, unsigned int length, float *position, float *quaternion, float *scale, unsigned int id, PxDefaultMemoryOutputStream *writeStream) {
  PxDefaultMemoryInputData readBuffer(data, length);
  PxConvexMesh *convexMesh = physics->createConvexMesh(readBuffer);

  PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
  PxTransform transform(PxVec3(position[0], position[1], position[2]), PxQuat(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
  PxMeshScale scaleObject(PxVec3(scale[0], scale[1], scale[2]));
  PxConvexMeshGeometry geometry(convexMesh, scaleObject);
  PxRigidDynamic *mesh = PxCreateDynamic(*physics, transform, geometry, *material, 1);
  mesh->userData = (void *)id;
  scene->addActor(*mesh);
  actors.push_back(mesh);

  if (writeStream) {
    delete writeStream;
  }
}
void PScene::enableActor(unsigned int id) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    PxScene *actorScene = actor->getScene();
    if (actorScene == nullptr) {
      scene->addActor(*actor);
    } else {
      std::cerr << "disable physics actor already had a scene " << id << std::endl;
    }
  } else {
    std::cerr << "enable unknown actor id " << id << std::endl;
  }
}
void PScene::disableActor(unsigned int id) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    PxScene *actorScene = actor->getScene();
    if (actorScene != nullptr) {
      actorScene->removeActor(*actor);
    } else {
      std::cerr << "disable physics actor id had no scene " << id << std::endl;
    }
  } else {
    std::cerr << "disable physics actor id " << id << std::endl;
  }
}
void PScene::disableGeometry(unsigned int id) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    // actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

    PxRigidBody *body = dynamic_cast<PxRigidBody *>(actor);
    /* if (body) {
      body->setLinearVelocity(PxVec3(0, 0, 0), false);
      body->setAngularVelocity(PxVec3(0, 0, 0), false);
    } */

    PxShape *shapes[32];
    for (int j = 0;; j++) {
      memset(shapes, 0, sizeof(shapes));
      if (actor->getShapes(shapes, 32, j * 32) == 0) {
        break;
      }
      for (int i = 0; i < 32; ++i) {
        if (shapes[i] == nullptr) {
          break;
        }

        PxShape *rigidShape = shapes[i];
        rigidShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
      }
    }
  } else {
    std::cerr << "disable unknown actor id " << id << std::endl;
  }
}
void PScene::enableGeometry(unsigned int id) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    
    // actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);

    PxShape *shapes[32];
    for (int j = 0; ; j++) {
      memset(shapes, 0, sizeof(shapes));
      if (actor->getShapes(shapes, 32, j * 32) == 0) {
        break;
      }
      for (int i = 0; i < 32; ++i) {
        if (shapes[i] == nullptr) {
          break;
        }

        PxShape *rigidShape = shapes[i];
        rigidShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
      }
    }
  } else {
    std::cerr << "enable unknown actor id " << id << std::endl;
  }
}
void PScene::disableGeometryQueries(unsigned int id) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    constexpr int numShapes = 32;
    PxShape *shapes[numShapes];
    for (int j = 0; ; j++) {
      memset(shapes, 0, sizeof(shapes));
      if (actor->getShapes(shapes, numShapes, j * numShapes) == 0) {
        break;
      }
      for (int i = 0; i < numShapes; ++i) {
        if (shapes[i] == nullptr) {
          break;
        }

        PxShape *rigidShape = shapes[i];
        rigidShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
      
        // std::cout << "disable queries for shape " << (unsigned int)actor->userData << " " << (uint32_t)rigidShape << " " << rigidShape->getFlags().isSet(PxShapeFlag::eSCENE_QUERY_SHAPE) << std::endl; // XXX
      }
    }
  } else {
    std::cerr << "disable queries unknown actor id " << id << std::endl;
  }
}
void PScene::enableGeometryQueries(unsigned int id) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    constexpr int numShapes = 32;
    PxShape *shapes[numShapes];
    for (int j = 0; ; j++) {
      memset(shapes, 0, sizeof(shapes));
      if (actor->getShapes(shapes, numShapes, j * numShapes) == 0) {
        break;
      }
      for (int i = 0; i < numShapes; ++i) {
        if (shapes[i] == nullptr) {
          break;
        }

        PxShape *rigidShape = shapes[i];
        rigidShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
      }
    }
  } else {
    std::cerr << "enable queries unknown actor id " << id << std::endl;
  }
}
void PScene::setTransform(unsigned int id, float *positions, float *quaternions, float *scales, bool autoWake) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    PxTransform transform(
      PxVec3(positions[0], positions[1], positions[2]),
      PxQuat(quaternions[0], quaternions[1], quaternions[2], quaternions[3])
    );
    actor->setGlobalPose(transform, autoWake);
  } else {
    std::cerr << "set transform unknown actor id " << id << std::endl;
  }
}
void PScene::getGlobalPosition(unsigned int id, float *positions) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    PxRigidBody *body = dynamic_cast<PxRigidBody *>(actor);
    if (body) {
      PxVec3 position = actor->getGlobalPose().p;
      positions[0]  = position[0]; 
      positions[1]  = position[1]; 
      positions[2]  = position[2]; 
    }
  } else {
    std::cerr << "get position unknown actor id " << id << std::endl;
    positions[0] = 0;
    positions[1] = 0;
    positions[2] = 0;
  }
}
void PScene::getVelocity(unsigned int id, float *velocities) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    PxRigidBody *body = dynamic_cast<PxRigidBody *>(actor);
    if (body) {
      PxVec3 linearVelocity = body->getLinearVelocity();
      velocities[0] = linearVelocity.x;
      velocities[1] = linearVelocity.y;
      velocities[2] = linearVelocity.z;
    }
  } else {
    std::cerr << "get velocity unknown actor id " << id << std::endl;
    velocities[0] = 0;
    velocities[1] = 0;
    velocities[2] = 0;
  }
}
void PScene::setVelocity(unsigned int id, float *velocities, bool autoWake) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    PxRigidBody *body = dynamic_cast<PxRigidBody *>(actor);
    if (body) {
      body->setLinearVelocity(PxVec3(velocities[0], velocities[1], velocities[2]), autoWake);
    }
  } else {
    std::cerr << "set linear velocity unknown actor id " << id << std::endl;
  }
}
void PScene::setAngularVel(unsigned int id, float *velocities, bool autoWake) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    PxRigidBody *body = dynamic_cast<PxRigidBody *>(actor);
    if (body) {
      body->setAngularVelocity(PxVec3(velocities[0], velocities[1], velocities[2]), autoWake);
    }
  } else {
    std::cerr << "set angular velocity unknown actor id " << id << std::endl;
  }
}
void PScene::setLinearLockFlags(unsigned int id, bool x, bool y, bool z) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    PxRigidDynamic *actorDynamic = dynamic_cast<PxRigidDynamic *>(actor);
    if (actorDynamic != nullptr) {
      PxRigidDynamicLockFlags flags = actorDynamic->getRigidDynamicLockFlags();
      flags &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
      flags &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
      flags &= ~PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
      
      if (!x) {
        flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
      }
      if (!y) {
        flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
      }
      if (!z) {
        flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
      }

      actorDynamic->setRigidDynamicLockFlags(flags);
    } else {
      std::cerr << "set linear lock flags non-dynamic actor id " << id << std::endl;
    }
  } else {
    std::cerr << "set linear lock flags unknown actor id " << id << std::endl;
  }
}
void PScene::setAngularLockFlags(unsigned int id, bool x, bool y, bool z) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    PxRigidDynamic *actorDynamic = dynamic_cast<PxRigidDynamic *>(actor);
    if (actorDynamic != nullptr) {
      PxRigidDynamicLockFlags flags = actorDynamic->getRigidDynamicLockFlags();
      flags &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
      flags &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
      flags &= ~PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

      if (!x) {
        flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
      }
      if (!y) {
        flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
      }
      if (!z) {
        flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
      }

      actorDynamic->setRigidDynamicLockFlags(flags);
    } else {
      std::cerr << "set angular lock flags non-dynamic actor id " << id << std::endl;
    }
  } else {
    std::cerr << "set angular lock flags unknown actor id " << id << std::endl;
  }
}
void PScene::setMassAndInertia(unsigned int id, float mass, float *inertia) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    PxRigidBody *body = dynamic_cast<PxRigidBody *>(actor);
    if (body != nullptr) {
      body->setMass(mass);
      body->setMassSpaceInertiaTensor(PxVec3{inertia[0], inertia[1], inertia[2]});
    } else {
      std::cerr << "set mass and inertia non-rigid body actor id " << id << std::endl;
    }
  } else {
    std::cerr << "set mass and inertia unknown actor id " << id << std::endl;
  }
}
void PScene::setGravityEnabled(unsigned int id, bool enabled) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !enabled);
  } else {
    std::cerr << "set gravity enabled unknown actor id " << id << std::endl;
  }
}
void PScene::removeGeometry(unsigned int id) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;

    PxScene *scene = actor->getScene();
    if (scene != nullptr) {
      scene->removeActor(*actor);
    }
    actor->release();
    actors.erase(actorIter);
    simulationEventCallback->stateBitfields.erase(id);
  } else {
    std::cerr << "remove unknown actor id " << id << std::endl;
  }
}
PxController *PScene::createCharacterController(float radius, float height, float contactOffset, float stepOffset, float *position, float *mat) {
  PxCapsuleControllerDesc desc{};
  desc.radius = radius;
  desc.height = height;
  desc.upDirection = PxVec3{0, 1, 0};
  desc.contactOffset = contactOffset;
  desc.stepOffset = stepOffset;
  // desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
  desc.material = physics->createMaterial(mat[0], mat[1], mat[2]);
  PxController *characterController = controllerManager->createController(desc);
  characterController->setPosition(PxExtendedVec3{position[0], position[1], position[2]});
  return characterController;
}
void PScene::destroyCharacterController(PxController *characterController) {
  characterController->release();
}
unsigned int PScene::moveCharacterController(PxController *characterController, float *displacement, float minDist, float elapsedTime, float *positionOut) {
  PxVec3 disp{
    displacement[0],
    displacement[1],
    displacement[2]
  };
  // PxF32 minDist = 0;
  // PxF32 elapsedTime = 0;
  PxControllerFilters controllerFilters{};
  PxObstacleContext *obstacles = nullptr;
  PxControllerCollisionFlags collisionFlags = characterController->move(
    disp,
    minDist,
    elapsedTime,
    controllerFilters,
    obstacles
  );

  const PxExtendedVec3 &pos = characterController->getPosition();
  positionOut[0] = pos.x;
  positionOut[1] = pos.y;
  positionOut[2] = pos.z;

  unsigned int flags = 0;
  if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
    flags |= (1 << 0);
    // characterController->setPosition(characterController->getPosition() + PxVec3(0, -0.01, 0));
  }
  if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_SIDES) {
    flags |= (1 << 1);
    // characterController->setPosition(characterController->getPosition() + PxVec3(0, -0.01, 0));
  }
  if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_UP) {
    flags |= (1 << 2);
    // characterController->setPosition(characterController->getPosition() + PxVec3(0, -0.01, 0));
  }
  return flags;
}
void PScene::setCharacterControllerPosition(PxController *characterController, float *position) {
  characterController->setPosition(PxExtendedVec3{position[0], position[1], position[2]});
}

const float boxPositions[] = {0.5,0.5,0.5,0.5,0.5,-0.5,0.5,-0.5,0.5,0.5,-0.5,-0.5,-0.5,0.5,-0.5,-0.5,0.5,0.5,-0.5,-0.5,-0.5,-0.5,-0.5,0.5,-0.5,0.5,-0.5,0.5,0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,0.5,-0.5,-0.5,0.5,0.5,-0.5,0.5,-0.5,-0.5,-0.5,0.5,-0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,0.5,-0.5,-0.5,0.5,0.5,-0.5,0.5,0.5,0.5,-0.5,-0.5,0.5,-0.5,0.5,-0.5,-0.5,-0.5,-0.5,-0.5};
const unsigned int boxIndices[] = {0,2,1,2,3,1,4,6,5,6,7,5,8,10,9,10,11,9,12,14,13,14,15,13,16,18,17,18,19,17,20,22,21,22,23,21};

bool PScene::getGeometry(unsigned int id, float *positions, unsigned int &numPositions, unsigned int *indices, unsigned int &numIndices, float *bounds) {
  numPositions = 0;
  numIndices = 0;

  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    unsigned int numShapes = actor->getNbShapes();
    if (numShapes == 1) {
      PxShape *shapes[1];
      actor->getShapes(shapes, sizeof(shapes)/sizeof(shapes[0]), 0);
      PxShape *shape = shapes[0];
      PxGeometryHolder geometryHolder = shape->getGeometry();
      PxGeometryType::Enum geometryType = geometryHolder.getType();
      switch (geometryType) {
        case PxGeometryType::Enum::eBOX: {
          // std::cout << "physics type 1.1" << std::endl;

          // const PxBoxGeometry &geometry = geometryHolder.box();
          // const PxVec3 &halfExtents = geometry.halfExtents;

          for (unsigned int i = 0; i < sizeof(boxPositions)/sizeof(boxPositions[0]); i += 3) {
            positions[numPositions++] = boxPositions[i] * 2;
            positions[numPositions++] = boxPositions[i+1] * 2;
            positions[numPositions++] = boxPositions[i+2] * 2;
          }

          memcpy(indices, boxIndices, sizeof(boxIndices));
          numIndices = sizeof(boxIndices)/sizeof(boxIndices[0]);

          {
            PxBoxGeometry &geometry = geometryHolder.box();
            PxTransform actorPose = actor->getGlobalPose();
            PxBounds3 actorBounds = PxGeometryQuery::getWorldBounds(geometry, actorPose, 1.0f);
            bounds[0] = actorBounds.minimum.x;
            bounds[1] = actorBounds.minimum.y;
            bounds[2] = actorBounds.minimum.z;
            bounds[3] = actorBounds.maximum.x;
            bounds[4] = actorBounds.maximum.y;
            bounds[5] = actorBounds.maximum.z;
          }

          return true;
        }
        case PxGeometryType::Enum::eCAPSULE: {
          // std::cout << "physics type 1.2" << std::endl;

          // XXX get the capsule geometry, and fill in the positions and indices
          /* const PxCapsuleGeometry &geometry = geometryHolder.capsule();
          const PxReal radius = geometry.radius;
          const PxReal halfHeight = geometry.halfHeight; */

          {
            PxCapsuleGeometry &geometry = geometryHolder.capsule();
            PxTransform actorPose = actor->getGlobalPose();
            PxBounds3 actorBounds = PxGeometryQuery::getWorldBounds(geometry, actorPose, 1.0f);
            bounds[0] = actorBounds.minimum.x;
            bounds[1] = actorBounds.minimum.y;
            bounds[2] = actorBounds.minimum.z;
            bounds[3] = actorBounds.maximum.x;
            bounds[4] = actorBounds.maximum.y;
            bounds[5] = actorBounds.maximum.z;
          }

          return true;
        }
        case PxGeometryType::Enum::eCONVEXMESH: {
          // std::cout << "physics type 2" << std::endl;

          PxConvexMeshGeometry &geometry = geometryHolder.convexMesh();
          PxConvexMesh *convexMesh = geometry.convexMesh;
          const PxVec3 *convexVerts = convexMesh->getVertices();
          // unsigned int numVertices = convexMesh->getNbVertices();
          unsigned int nbPolygons = convexMesh->getNbPolygons();
          const unsigned char *indexBuffer = convexMesh->getIndexBuffer();

          PxU32 totalNbTris = 0;
          PxU32 totalNbVerts = 0;
          for(PxU32 i = 0; i < nbPolygons; i++) {
            PxHullPolygon data;
            convexMesh->getPolygonData(i, data);
            totalNbVerts += data.mNbVerts;
            totalNbTris += data.mNbVerts - 2;
          }

          PxVec3 *vertices = (PxVec3 *)positions;
          PxU32 *triangles = indices;

          PxU32 offset = 0;
          for (unsigned int i = 0; i < nbPolygons; i++) {
            PxHullPolygon face;
            convexMesh->getPolygonData(i, face);

            const PxU8 *faceIndices = indexBuffer + face.mIndexBase;
            for (PxU32 j = 0; j < face.mNbVerts; j++) {
              vertices[offset+j] = convexVerts[faceIndices[j]];
            }

            for (PxU32 j = 2; j < face.mNbVerts; j++) {
              triangles[numIndices++] = PxU32(offset);
              triangles[numIndices++] = PxU32(offset+j);
              triangles[numIndices++] = PxU32(offset+j-1);
            }

            offset += face.mNbVerts;
          }
          numPositions = offset;

          {
            PxConvexMeshGeometry &geometry = geometryHolder.convexMesh();
            PxTransform actorPose = actor->getGlobalPose();
            PxBounds3 actorBounds = PxGeometryQuery::getWorldBounds(geometry, actorPose, 1.0f);
            bounds[0] = actorBounds.minimum.x;
            bounds[1] = actorBounds.minimum.y;
            bounds[2] = actorBounds.minimum.z;
            bounds[3] = actorBounds.maximum.x;
            bounds[4] = actorBounds.maximum.y;
            bounds[5] = actorBounds.maximum.z;
          }

          return true;
        }
        case PxGeometryType::Enum::eTRIANGLEMESH: {
          // std::cout << "physics type 3" << std::endl;

          PxTriangleMeshGeometry &geometry = geometryHolder.triangleMesh();
          PxTriangleMesh *triangleMesh = geometry.triangleMesh;
          const unsigned int numVertices = triangleMesh->getNbVertices();
          const PxVec3 *vertices = triangleMesh->getVertices();
          const unsigned int nbTris = triangleMesh->getNbTriangles();
          const void *triangles = triangleMesh->getTriangles();
          const PxTriangleMeshFlags &flags = triangleMesh->getTriangleMeshFlags();
          const bool has16BitIndices = flags & PxTriangleMeshFlag::e16_BIT_INDICES ? true : false;

          memcpy(positions, vertices, numVertices * sizeof(vertices[0]));
          numPositions = numVertices * 3;

          if (has16BitIndices) {
            // std::cout << "physics type 3.1" << std::endl;
            unsigned short *triangles16 = (unsigned short *)triangles;
            for (unsigned int i = 0; i < nbTris; i++) {
              indices[numIndices++] = triangles16[i*3+0];
              indices[numIndices++] = triangles16[i*3+1];
              indices[numIndices++] = triangles16[i*3+2];
            }
          } else {
            // std::cout << "physics type 3.2" << std::endl;
            unsigned int *triangles32 = (unsigned int *)triangles;
            for (unsigned int i = 0; i < nbTris; i++) {
              indices[numIndices++] = triangles32[i*3+0];
              indices[numIndices++] = triangles32[i*3+1];
              indices[numIndices++] = triangles32[i*3+2];
            }
          }

          {
            PxTriangleMeshGeometry &geometry = geometryHolder.triangleMesh();
            PxTransform actorPose = actor->getGlobalPose();
            PxBounds3 actorBounds = PxGeometryQuery::getWorldBounds(geometry, actorPose, 1.0f);
            bounds[0] = actorBounds.minimum.x;
            bounds[1] = actorBounds.minimum.y;
            bounds[2] = actorBounds.minimum.z;
            bounds[3] = actorBounds.maximum.x;
            bounds[4] = actorBounds.maximum.y;
            bounds[5] = actorBounds.maximum.z;
          }

          return true;
        }
        default: {
          std::cerr << "unknown geometry type for actor id " << id << " : " << (unsigned int)geometryType << std::endl;
          return false;
        }
      }
    } else {
      std::cerr << "no shapes for actor id " << id << std::endl;
      return false;
    }
  } else {
    std::cerr << "get geometry unknown actor id " << id << std::endl;
    return false;
  }
}

PxBounds3 getActorBounds(PxRigidActor *actor) {
  PxShape *shapes[1];
  actor->getShapes(shapes, sizeof(shapes)/sizeof(shapes[0]), 0);
  PxShape *shape = shapes[0];
  PxGeometryHolder geometryHolder = shape->getGeometry();
  PxGeometryType::Enum geometryType = geometryHolder.getType();
  switch (geometryType) {
    case PxGeometryType::Enum::eBOX: {
      PxBoxGeometry &geometry = geometryHolder.box();
      PxTransform actorPose = actor->getGlobalPose();
      PxBounds3 actorBounds = PxGeometryQuery::getWorldBounds(geometry, actorPose, 1.0f);
      return actorBounds;
    }
    case PxGeometryType::Enum::eCAPSULE: {
      // std::cout << "physics type 1.2" << std::endl;

      // XXX get the capsule geometry, and fill in the positions and indices
      /* const PxCapsuleGeometry &geometry = geometryHolder.capsule();
      const PxReal radius = geometry.radius;
      const PxReal halfHeight = geometry.halfHeight; */

      PxCapsuleGeometry &geometry = geometryHolder.capsule();
      PxTransform actorPose = actor->getGlobalPose();
      PxBounds3 actorBounds = PxGeometryQuery::getWorldBounds(geometry, actorPose, 1.0f);
      return actorBounds;
    }
    case PxGeometryType::Enum::eCONVEXMESH: {
      // std::cout << "physics type 2" << std::endl;

      PxConvexMeshGeometry &geometry = geometryHolder.convexMesh();
      PxTransform actorPose = actor->getGlobalPose();
      PxBounds3 actorBounds = PxGeometryQuery::getWorldBounds(geometry, actorPose, 1.0f);
      return actorBounds;
    }
    case PxGeometryType::Enum::eTRIANGLEMESH: {
      // std::cout << "physics type 3" << std::endl;

      PxTriangleMeshGeometry &geometry = geometryHolder.triangleMesh();
      PxTransform actorPose = actor->getGlobalPose();
      PxBounds3 actorBounds = PxGeometryQuery::getWorldBounds(geometry, actorPose, 1.0f);
      return actorBounds;
    }
    case PxGeometryType::Enum::eINVALID:
    case PxGeometryType::Enum::eSPHERE:
    case PxGeometryType::Enum::ePLANE:
    case PxGeometryType::Enum::eHEIGHTFIELD:
    case PxGeometryType::Enum::eGEOMETRY_COUNT:
    {
      break;
    }
  }
  return PxBounds3();
}

bool PScene::getBounds(unsigned int id, float *bounds) {
  auto actorIter = std::find_if(actors.begin(), actors.end(), [&](PxRigidActor *actor) -> bool {
    return (unsigned int)actor->userData == id;
  });
  if (actorIter != actors.end()) {
    PxRigidActor *actor = *actorIter;
    unsigned int numShapes = actor->getNbShapes();
    if (numShapes == 1) {
      PxBounds3 actorBounds = getActorBounds(actor);

      bounds[0] = actorBounds.minimum.x;
      bounds[1] = actorBounds.minimum.y;
      bounds[2] = actorBounds.minimum.z;
      bounds[3] = actorBounds.maximum.x;
      bounds[4] = actorBounds.maximum.y;
      bounds[5] = actorBounds.maximum.z;

      return true;
    } else {
      std::cerr << "get bounds no shapes for actor id " << id << std::endl;
      return false;
    }
  } else {
    std::cerr << "get bounds get geometry unknown actor id " << id << std::endl;
    return false;
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

      if (shape->getFlags().isSet(PxShapeFlag::eSCENE_QUERY_SHAPE)) {
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
}

void PScene::overlap(PxGeometry *geom, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int &hit, unsigned int &id) {
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
  unsigned int localId = 0;
  {
      for (unsigned int i = 0; i < actors.size(); i++) {
        PxRigidActor *actor = actors[i];
        PxShape *shape;
        actor->getShapes(&shape, 1);

        if (shape->getFlags().isSet(PxShapeFlag::eSCENE_QUERY_SHAPE)) {
          PxGeometryHolder holder = shape->getGeometry();
          PxGeometry &geometry = holder.any();

          PxTransform meshPose2 = actor->getGlobalPose();
          PxTransform meshPose3 = meshPose * meshPose2;

          PxVec3 directionVec;
          PxReal depthFloat;
          bool result = PxGeometryQuery::overlap(*geom, geomPose, geometry, meshPose3);
          if (result) {
            anyHadHit = true;
            id = (unsigned int)actor->userData;
            break;
          }
        }
      }
  }
  if (anyHadHit) {
    hit = 1;
  } else {
    hit = 0;
  }
}

void PScene::overlapBox(float hx, float hy, float hz, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int &hit, unsigned int &id) {
  PxBoxGeometry geom(hx, hy, hz);
  PScene::overlap(&geom, position, quaternion, meshPosition, meshQuaternion, hit, id);
}

void PScene::overlapCapsule(float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int &hit, unsigned int &id) {
  PxCapsuleGeometry geom(radius, halfHeight);
  PScene::overlap(&geom, position, quaternion, meshPosition, meshQuaternion, hit, id);
}

void PScene::collide(PxGeometry *geom, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int &hit, float *direction, unsigned int &grounded, unsigned int &id) {
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
  unsigned int localId = 0;
  {
    for (unsigned int i = 0; i < maxIter; i++) {
      bool hadHit = false;
        
      PxVec3 largestDirectionVec;
      unsigned int largestId = 0;
      for (unsigned int i = 0; i < actors.size(); i++) {
        PxRigidActor *actor = actors[i];
        PxShape *shape;
        actor->getShapes(&shape, 1);

        if (shape->getFlags().isSet(PxShapeFlag::eSCENE_QUERY_SHAPE)) {
          PxGeometryHolder holder = shape->getGeometry();
          PxGeometry &geometry = holder.any();

          PxTransform meshPose2 = actor->getGlobalPose();
          PxTransform meshPose3 = meshPose * meshPose2;

          PxVec3 directionVec;
          PxReal depthFloat;
          bool result = PxGeometryQuery::computePenetration(directionVec, depthFloat, *geom, geomPose, geometry, meshPose3);
          if (result) {
            // std::cout << "collide shape " << (unsigned int)actor->userData << " " << (uint32_t)shape << " " << shape->getFlags().isSet(PxShapeFlag::eSCENE_QUERY_SHAPE) << std::endl; // XXX
            
            anyHadHit = true;
            hadHit = true;
            anyHadGrounded = anyHadGrounded || directionVec.y > 0;

            directionVec *= depthFloat;
            if (largestId == 0 || directionVec.magnitudeSquared() > largestDirectionVec.magnitudeSquared()) {
              largestDirectionVec = directionVec;
              largestId = (unsigned int)actor->userData;
            }
          }
        }
      }
      if (hadHit) {
        offset += Vec(largestDirectionVec.x, largestDirectionVec.y, largestDirectionVec.z);
        geomPose.p += largestDirectionVec;
        localId = largestId;
        
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
    id = localId;
  } else {
    hit = 0;
  }
}

void PScene::collideBox(float hx, float hy, float hz, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int &hit, float *direction, unsigned int &grounded, unsigned int &id) {
  PxBoxGeometry geom(hx, hy, hz);
  PScene::collide(&geom, position, quaternion, meshPosition, meshQuaternion, maxIter, hit, direction, grounded, id);
}

void PScene::collideCapsule(float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int &hit, float *direction, unsigned int &grounded, unsigned int &id) {
  PxCapsuleGeometry geom(radius, halfHeight);
  PScene::collide(&geom, position, quaternion, meshPosition, meshQuaternion, maxIter, hit, direction, grounded, id);
}

void PScene::getCollisionObject(float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int &hit, unsigned int &id) {
  PxCapsuleGeometry geom(radius, halfHeight);
  PxTransform geomPose(
    PxVec3{position[0], position[1], position[2]},
    PxQuat{quaternion[0], quaternion[1], quaternion[2], quaternion[3]}
  );
  PxTransform meshPose{
    PxVec3{meshPosition[0], meshPosition[1], meshPosition[2]},
    PxQuat{meshQuaternion[0], meshQuaternion[1], meshQuaternion[2], meshQuaternion[3]}
  };

  PxVec3 smallestSizeVec;
  unsigned int largestId = 0;
  for (unsigned int i = 0; i < actors.size(); i++) {
    PxRigidActor *actor = actors[i];
    PxShape *shape;
    actor->getShapes(&shape, 1);

    if (shape->getFlags().isSet(PxShapeFlag::eSCENE_QUERY_SHAPE)) {
      PxGeometryHolder holder = shape->getGeometry();
      PxGeometry &geometry = holder.any();

      PxTransform meshPose2 = actor->getGlobalPose();
      PxTransform meshPose3 = meshPose * meshPose2;

      bool result = PxGeometryQuery::overlap(geom, geomPose, geometry, meshPose3);
      if (result) {
        hit = 1;

        const PxBounds3 &bounds = getActorBounds(actor);
        PxVec3 sizeVec{
          bounds.maximum.x - bounds.minimum.x,
          bounds.maximum.y - bounds.minimum.y,
          bounds.maximum.z - bounds.minimum.z
        };
        if (largestId == 0 || sizeVec.magnitudeSquared() < smallestSizeVec.magnitudeSquared()) {
          smallestSizeVec = sizeVec;
          id = (unsigned int)actor->userData;
        }
      }
    }
  }
}