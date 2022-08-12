// #include "DirectXMath.h"
/* #include "PhysX/physx/include/geometry/PxMeshQuery.h"
#include "geometry/PxCapsuleGeometry.h"
#include "foundation/PxTransform.h"
 */
#include "physics.h"
#include <string>
#include <iostream>
#include "geometry/PxHeightFieldSample.h"
#include "geometry/PxHeightFieldDesc.h"
#include "geometry/PxHeightFieldGeometry.h"

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
void PBase::addHeightFieldGeometry(unsigned int id) {
  // args
  float hfScale = 10.;
  const PxReal heightScale = 0.005f;
  PxVec3 midPoint(0, 0, 0);
  PxMaterial *material = physics->createMaterial(0.5f, 0.5f, 0.1f);
  // end args

	unsigned int hfWidth = 10;
	unsigned int hfHeight = 10;

  PxTransform pose;
	pose.p = midPoint + PxVec3(-((hfWidth - 1)*0.5f*hfScale), 0, -((hfHeight - 1)*0.5f*hfScale));

	PxU32 hfNumVerts = hfWidth*hfHeight;

	PxHeightFieldSample* samples = new PxHeightFieldSample[hfNumVerts];
	memset(samples,0,hfNumVerts*sizeof(PxHeightFieldSample));

	PxHeightFieldDesc hfDesc;
	//hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = hfWidth;
	hfDesc.nbRows = hfHeight;
	hfDesc.samples.data = samples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxHeightField* heightField = cooking->createHeightField(hfDesc, physics->getPhysicsInsertionCallback());

	PxRigidStatic* actor = physics->createRigidStatic(pose);

	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, hfScale, hfScale);
	PxShape* hfShape = PxRigidActorExt::createExclusiveShape(*actor, hfGeom, *material);

	// hfShape->setQueryFilterData(queryFilterData);
	// hfShape->setSimulationFilterData(simulationFilterData);

  actor->userData = (void *)id;
  scene->addActor(*actor);
  actors->push_back(actor);

	delete[] samples;
}

//

PBase *physicsBase = nullptr;