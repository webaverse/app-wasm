// #include "DirectXMath.h"
/* #include "PhysX/physx/include/geometry/PxMeshQuery.h"
#include "geometry/PxCapsuleGeometry.h"
#include "foundation/PxTransform.h"
 */
#include "physics.h"
#include <string>
#include <iostream>

//

using namespace physx;

//

PBase::PBase() {
  allocator = new PxDefaultAllocator();
  errorCallback = new PxDefaultErrorCallback();
  foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, *errorCallback);
  {
    PxCookingParams cookingParams(tolerancesScale);
    cookingParams.meshWeldTolerance = 0.15;
    // cookingParams.planeTolerance = 0;
    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
    // cookingParams.meshSizePerformanceTradeOff = 0;
    cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, cookingParams);
  }
}
PBase::~PBase() {}

//

void PBase::cookGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
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
void PBase::cookConvexGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uint8_t **data, unsigned int *length, PxDefaultMemoryOutputStream **writeStream) {
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

//

PBase *physicsBase = nullptr;