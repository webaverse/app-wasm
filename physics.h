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
#include "geometry/PxTriangleMeshGeometry.h"
#include "cooking/PxTriangleMeshDesc.h"
#include "cooking/PxCooking.h"
#include "extensions/PxTriangleMeshExt.h"
#include "extensions/PxSimpleFactory.h"
// #include "PxPhysicsAPI.h"
#include "PxQueryReport.h"
#include "geometry/PxGeometryQuery.h"
#include <list>
#include <map>
#include <set>
#include <deque>
#include <algorithm>
#include <iostream>

using namespace physx;

class PScene {
public:
  PScene();
  ~PScene();
  
  void addGeometry(int type, float *position, float *quaternion);
  void simulate(unsigned int *ids, float *positions, float *quaternion, float elapsedTime);
  
  PxDefaultAllocator *allocator = nullptr;
  PxDefaultErrorCallback *errorCallback = nullptr;
  PxFoundation *foundation = nullptr;
  PxPhysics *physics = nullptr;
  PxCooking *cooking = nullptr;
  PxScene *scene = nullptr;
};

#endif