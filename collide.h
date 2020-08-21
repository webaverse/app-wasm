// #include "DirectXMath.h"
/* #include "PhysX/physx/include/geometry/PxMeshQuery.h"
#include "geometry/PxCapsuleGeometry.h"
#include "foundation/PxTransform.h"
 */
#include "vector.h"
#include "march.h"
#include "PxPhysicsVersion.h"
#include "PxPhysics.h"
#include "extensions/PxDefaultStreams.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "cooking/PxTriangleMeshDesc.h"
#include "cooking/PxCooking.h"
#include "extensions/PxTriangleMeshExt.h"
#include "PxQueryReport.h"
#include "geometry/PxGeometryQuery.h"
#include <set>
#include <deque>
#include <algorithm>
#include <iostream>

using namespace physx;

const float subparcelRadius = std::sqrt(((float)SUBPARCEL_SIZE/2.0f)*((float)SUBPARCEL_SIZE/2.0f)*3.0f);

class GeometrySpec {
public:
  GeometrySpec(unsigned int meshId, PxTriangleMesh *triangleMesh, PxGeometry *meshGeom, Vec position, Quat quaternion, Sphere boundingSphere) :
    meshId(meshId), triangleMesh(triangleMesh), meshGeom(meshGeom), position(position), quaternion(quaternion), boundingSphere(boundingSphere) {}
  ~GeometrySpec() {
    delete meshGeom;
    if (triangleMesh) {
      triangleMesh->release();
    }
  }

  unsigned int meshId;
  PxTriangleMesh *triangleMesh;
  PxGeometry *meshGeom;
  Vec position;
  Quat quaternion;
  Sphere boundingSphere;
};

PxDefaultAllocator *gAllocator = nullptr;
PxDefaultErrorCallback *gErrorCallback = nullptr;
PxFoundation *gFoundation = nullptr;
PxPhysics *physics = nullptr;
PxCooking *cooking = nullptr;
std::set<GeometrySpec *> geometrySpecs;
std::set<GeometrySpec *> staticGeometrySpecs;
std::vector<std::set<GeometrySpec *> *> geometrySpecSets{
  &staticGeometrySpecs,
  &geometrySpecs,
};

void doInitPhysx() {
  gAllocator = new PxDefaultAllocator();
  gErrorCallback = new PxDefaultErrorCallback();
  gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *gAllocator, *gErrorCallback);
  PxTolerancesScale tolerancesScale;
  physics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, tolerancesScale);
  PxCookingParams cookingParams(tolerancesScale);
  // cookingParams.midphaseDesc = PxMeshMidPhase::eBVH34;
  cooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, cookingParams);
}

uintptr_t doRegisterBakedGeometry(unsigned int meshId, PxDefaultMemoryOutputStream *writeStream, float *meshPosition, float *meshQuaternion) {
  PxDefaultMemoryInputData readBuffer((PxU8 *)writeStream->getData(), writeStream->getSize());
  PxTriangleMesh *triangleMesh = physics->createTriangleMesh(readBuffer);
  delete writeStream;

  PxTriangleMeshGeometry *meshGeom = new PxTriangleMeshGeometry(triangleMesh);
  Sphere boundingSphere(meshPosition[0], meshPosition[1], meshPosition[2], subparcelRadius);
  GeometrySpec *geometrySpec = new GeometrySpec(meshId, triangleMesh, meshGeom, Vec(), Quat(), boundingSphere);
  geometrySpecs.insert(geometrySpec);
  return (uintptr_t)geometrySpec;
}

uintptr_t doRegisterBoxGeometry(unsigned int meshId, float *position, float *quaternion, float w, float h, float d) {
  Vec p(position[0], position[1], position[2]);
  Quat q(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
  Vec halfScale(w/2.0f, h/2.0f, d/2.0f);
  PxBoxGeometry *meshGeom = new PxBoxGeometry(halfScale.x, halfScale.y, halfScale.z);
  Sphere boundingSphere(0, 0, 0, halfScale.magnitude());
  GeometrySpec *geometrySpec = new GeometrySpec(meshId, nullptr, meshGeom, p, q, boundingSphere);
  staticGeometrySpecs.insert(geometrySpec);
  return (uintptr_t)geometrySpec;
}

uintptr_t doRegisterCapsuleGeometry(unsigned int meshId, float *position, float *quaternion, float radius, float halfHeight) {
  Vec p(position[0], position[1], position[2]);
  Quat q(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
  PxCapsuleGeometry *meshGeom = new PxCapsuleGeometry(radius, halfHeight);
  Sphere boundingSphere(0, 0, 0, radius + halfHeight);
  GeometrySpec *geometrySpec = new GeometrySpec(meshId, nullptr, meshGeom, p, q, boundingSphere);
  staticGeometrySpecs.insert(geometrySpec);
  return (uintptr_t)geometrySpec;
}

PxDefaultMemoryOutputStream *doBakeGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices) {
  PxVec3 *verts = (PxVec3 *)positions;
  PxU32 nbVerts = numPositions/3;
  PxU32 *indices32 = (PxU32 *)indices;
  PxU32 triCount = numIndices/3;

  PxTriangleMeshDesc meshDesc;
  meshDesc.points.count           = nbVerts;
  meshDesc.points.stride          = sizeof(PxVec3);
  meshDesc.points.data            = verts;

  meshDesc.triangles.count        = triCount;
  meshDesc.triangles.stride       = 3*sizeof(PxU32);
  meshDesc.triangles.data         = indices32;

  // std::cout << "bake geo 1" << std::endl;
  PxDefaultMemoryOutputStream *writeBuffer = new PxDefaultMemoryOutputStream();
  // std::cout << "bake geo 2" << std::endl;
  bool status = cooking->cookTriangleMesh(meshDesc, *writeBuffer);
  // std::cout << "bake geo 3 " << status << " " << (void *)writeBuffer << std::endl;
  if (status) {
    return writeBuffer;
  } else {
    delete writeBuffer;
    return nullptr;
  }
}

/* void doBakeConvexGeometry(float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices, uintptr_t &ptr, uintptr_t &data, size_t &size) {
  PxVec3 *verts = (PxVec3 *)positions;
  PxU32 nbVerts = numPositions/3;
  PxU32 *indices32 = (PxU32 *)indices;
  PxU32 triCount = numIndices/3;

  PxConvexMeshDesc meshDesc;
  meshDesc.points.count           = nbVerts;
  meshDesc.points.stride          = sizeof(PxVec3);
  meshDesc.points.data            = verts;

  meshDesc.triangles.count        = triCount;
  meshDesc.triangles.stride       = 3*sizeof(PxU32);
  meshDesc.triangles.data         = indices32;

  PxDefaultMemoryOutputStream *writeBuffer = new PxDefaultMemoryOutputStream();
  bool status = cooking->cookConvexMesh(meshDesc, *writeBuffer);
  if (status) {
    ptr = (uintptr_t)writeBuffer;
    data = (uintptr_t)writeBuffer->getData();
    size = writeBuffer->getSize();
  } else {
    delete writeBuffer;
    ptr = 0;
    data = 0;
    size = 0;
  }
} */

void doUnregisterGeometry(uintptr_t geometrySpecPtr) {
  GeometrySpec *geometrySpec = (GeometrySpec *)geometrySpecPtr;
  delete geometrySpec;
  for (std::set<GeometrySpec *> *geometrySpecSet : geometrySpecSets) {
    geometrySpecSet->erase(geometrySpec);
  }
}

void doRaycast(float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int &hit, float *position, float *normal, float &distance, unsigned int &meshId, unsigned int &faceIndex) {
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

  std::vector<GeometrySpec *> sortedGeometrySpecs;
  sortedGeometrySpecs.reserve(geometrySpecs.size());
  for (std::set<GeometrySpec *> *geometrySpecSet : geometrySpecSets) {
    for (GeometrySpec *geometrySpec : *geometrySpecSet) {
      Sphere sphere(
        (geometrySpec->boundingSphere.center.clone().applyQuaternion(geometrySpec->quaternion) + geometrySpec->position)
          .applyQuaternion(q) + p,
        geometrySpec->boundingSphere.radius
      );
      if (ray.intersectsSphere(sphere)) {
        sortedGeometrySpecs.push_back(geometrySpec);
      }
    }
  }
  /* std::sort(sortedGeometrySpecs.begin(), sortedGeometrySpecs.end(), [](const std::pair<float, GeometrySpec *> &a, const std::pair<float, GeometrySpec *> &b) -> bool {
    return a.first < b.first;
  }); */

  hit = 0;
  for (GeometrySpec *geometrySpec : sortedGeometrySpecs) {
    const unsigned int &meshIdData = geometrySpec->meshId;
    PxGeometry *meshGeom = geometrySpec->meshGeom;
    PxTransform meshPose2{
      PxVec3{geometrySpec->position.x, geometrySpec->position.y, geometrySpec->position.z},
      PxQuat{geometrySpec->quaternion.x, geometrySpec->quaternion.y, geometrySpec->quaternion.z, geometrySpec->quaternion.w}
    };
    PxTransform meshPose3 = meshPose * meshPose2;
    PxTransform meshPose4 = meshPose2 * meshPose;

    PxU32 hitCount = PxGeometryQuery::raycast(originVec, directionVec,
                                              *meshGeom,
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
      meshId = meshIdData;
      faceIndex = hitInfo.faceIndex;
    }
  }
}

void doCollide(float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int &hit, float *direction, unsigned int &grounded) {
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

  std::vector<std::tuple<bool, float, GeometrySpec *>> sortedGeometrySpecs;
  unsigned int totalNumSpecs = 0;
  for (std::set<GeometrySpec *> *geometrySpecSet : geometrySpecSets) {
    totalNumSpecs += geometrySpecSet->size();
  }
  sortedGeometrySpecs.reserve(totalNumSpecs);

  Vec offset(0, 0, 0);
  bool anyHadHit = false;
  bool anyHadGrounded = false;
  for (unsigned int i = 0; i < maxIter; i++) {
    Vec capsulePosition(geomPose.p.x, geomPose.p.y, geomPose.p.z);
    sortedGeometrySpecs.clear();

    for (std::set<GeometrySpec *> *geometrySpecSet : geometrySpecSets) {
      for (GeometrySpec *geometrySpec : *geometrySpecSet) {
        Vec spherePosition = (geometrySpec->boundingSphere.center.clone().applyQuaternion(geometrySpec->quaternion) + geometrySpec->position)
          .applyQuaternion(q) + p;
        float distance = spherePosition.distanceTo(capsulePosition);
        if (distance < (geometrySpec->boundingSphere.radius + halfHeight + radius)) {
          sortedGeometrySpecs.push_back(std::tuple<bool, float, GeometrySpec *>(geometrySpecSet == &staticGeometrySpecs, distance, geometrySpec));
        }
      }
    }
    std::sort(sortedGeometrySpecs.begin(), sortedGeometrySpecs.end(), [](const std::tuple<bool, float, GeometrySpec *> &a, const std::tuple<bool, float, GeometrySpec *> &b) -> bool {
      const bool &aStatic = std::get<0>(a);
      const bool &bStatic = std::get<0>(b);
      if (aStatic != bStatic) {
        return aStatic > bStatic;
      } else {
        const float &aDistance = std::get<1>(a);
        const float &bDistance = std::get<1>(b);
        return aDistance < bDistance;
      }
    });

    bool hadHit = false;
    for (const std::tuple<bool, float, GeometrySpec *> &t : sortedGeometrySpecs) {
      GeometrySpec *geometrySpec = std::get<2>(t);
      PxGeometry *meshGeom = geometrySpec->meshGeom;
      PxTransform meshPose2{
        PxVec3{geometrySpec->position.x, geometrySpec->position.y, geometrySpec->position.z},
        PxQuat{geometrySpec->quaternion.x, geometrySpec->quaternion.y, geometrySpec->quaternion.z, geometrySpec->quaternion.w}
      };
      PxTransform meshPose3 = meshPose * meshPose2;

      PxVec3 directionVec;
      PxReal depthFloat;
      bool result = PxGeometryQuery::computePenetration(directionVec, depthFloat, geom, geomPose, *meshGeom, meshPose3);
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

int PEEK_FACE_INDICES[] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,1,2,3,4,255,255,255,255,255,255,255,255,255,255,0,255,5,6,7,8,255,255,255,255,255,255,255,255,255,255,1,5,255,9,10,11,255,255,255,255,255,255,255,255,255,255,2,6,9,255,12,13,255,255,255,255,255,255,255,255,255,255,3,7,10,12,255,14,255,255,255,255,255,255,255,255,255,255,4,8,11,13,14,255};
/* (() => {
  const directionsLookup = {
    1: 'FRONT',
    2: 'BACK',
    3: 'LEFT',
    4: 'RIGHT',
    5: 'TOP',
    6: 'BOTTOM',
  };
  const indexDirections = [];

  const PEEK_FACE_INDICES = Array((6<<4|6)+1);

  for (let i = 0; i < (6<<4|6)+1; i++) {
    PEEK_FACE_INDICES[i] = 0xFF;
  }

  let peekIndex = 0;
  for (let i = 1; i <= 6; i++) {
    for (let j = 1; j <= 6; j++) {
      if (i != j) {
        const otherEntry = PEEK_FACE_INDICES[j << 4 | i];
        PEEK_FACE_INDICES[i << 4 | j] = otherEntry != 0xFF ? otherEntry : (() => {
          const newIndex = peekIndex++;
          indexDirections[newIndex] = directionsLookup[i] + ' -> ' + directionsLookup[j];
          return newIndex;
        })();
      }
    }
  }

  x = PEEK_FACE_INDICES;
  y = indexDirections;
})(); */
class PeekDirection {
public:
  Vec normal;
  int inormal[3];
  PEEK_FACES face;
};
PeekDirection PEEK_DIRECTIONS[6] = {
  {{0, 0, 1}, {0, 0, 1}, PEEK_FACES::FRONT},
  {{0, 0, -1}, {0, 0, -1}, PEEK_FACES::BACK},
  {{-1, 0, 0}, {-1, 0, 0}, PEEK_FACES::LEFT},
  {{1, 0, 0}, {1, 0, 0}, PEEK_FACES::RIGHT},
  {{0, 1, 0}, {0, 1, 0}, PEEK_FACES::TOP},
  {{0, -1, 0}, {0, -1, 0}, PEEK_FACES::BOTTOM},
};

class Group {
public:
  unsigned int start;
  unsigned int count;
  unsigned int materialIndex;
};
class GroupSet {
public:
  GroupSet(int x, int y, int z, int index, const Sphere &boundingSphere, unsigned char *peeks, Group *groups, unsigned int numGroups) :
    x(x), y(y), z(z), index(index), boundingSphere(boundingSphere)
  {
    memcpy(this->peeks, peeks, 15);
    this->groups.reserve(numGroups);
    for (unsigned int i = 0; i < numGroups; i++) {
      this->groups.push_back(groups[i]);
    }
  }

  int x;
  int y;
  int z;
  int index;
  Sphere boundingSphere;
  unsigned char peeks[15];
  std::vector<Group> groups;
};
class Culler {
public:
  Culler() {
    groupSets.reserve(512);
  }
  std::vector<GroupSet *> groupSets;
};
class CullResult {
public:
  CullResult(unsigned int start, unsigned int count, unsigned int materialIndex) : start(start), count(count), materialIndex(materialIndex) {}

  unsigned int start;
  unsigned int count;
  unsigned int materialIndex;
};

Culler *doMakeCuller() {
  return new Culler();
}
GroupSet *doRegisterGroupSet(Culler *culler, int x, int y, int z, float r, unsigned char *peeks, Group *groups, unsigned int numGroups) {
  GroupSet *groupSet = new GroupSet(
    x,
    y,
    z,
    getSubparcelIndex(x, y, z),
    Sphere(x*SUBPARCEL_SIZE + SUBPARCEL_SIZE/2, y*SUBPARCEL_SIZE + SUBPARCEL_SIZE/2, z*SUBPARCEL_SIZE + SUBPARCEL_SIZE/2, r),
    peeks,
    groups,
    numGroups
  );
  culler->groupSets.push_back(groupSet);
  return groupSet;
}
void doUnregisterGroupSet(Culler *culler, GroupSet *groupSet) {
  auto groupSetIter = std::find(culler->groupSets.begin(), culler->groupSets.end(), groupSet);
  culler->groupSets.erase(groupSetIter);
  delete groupSet;
}
void doCull(Culler *culler, float *positionData, float *matrixData, float slabRadius, CullResult *cullResults, unsigned int &numCullResults) {
  Vec position(positionData[0], positionData[1], positionData[2]);
  Frustum frustum;
  frustum.setFromMatrix(matrixData);
  std::vector<GroupSet *> &groupSets = culler->groupSets;

  // frustum cull
  std::vector<GroupSet *> frustumGroupSets;
  frustumGroupSets.reserve(groupSets.size());
  for (int i = 0; i < groupSets.size(); i++) {
    if (frustum.intersectsSphere(groupSets[i]->boundingSphere)) {
      frustumGroupSets.push_back(groupSets[i]);
    }
  }
  std::sort(frustumGroupSets.begin(), frustumGroupSets.end(), [&](GroupSet *a, GroupSet *b) -> bool {
    return a->boundingSphere.center.distanceTo(position) < b->boundingSphere.center.distanceTo(position);
  });

  // intialize queue
  std::deque<GroupSet *> queue;
  std::set<GroupSet *> seenQueue;
  for (int i = 0; i < frustumGroupSets.size(); i++) {
    GroupSet *groupSet = frustumGroupSets[i];
    if (groupSet->boundingSphere.center.distanceTo(position) < slabRadius*2.0f) {
      queue.push_back(groupSet);
      seenQueue.insert(groupSet);
    }
  }

  // run queue
  numCullResults = 0;
  while (queue.size() > 0) {
    GroupSet *groupSet = queue.front();
    queue.pop_front();

    for (const Group &group : groupSet->groups) {
      CullResult &cullResult = cullResults[numCullResults++];
      cullResult.start = group.start;
      cullResult.count = group.count;
      cullResult.materialIndex = group.materialIndex;
    }

    for (const PeekDirection &enterPeekDirection : PEEK_DIRECTIONS) {
      const Vec &enterNormal = enterPeekDirection.normal;
      const int *enterINormal = enterPeekDirection.inormal;
      const PEEK_FACES &enterFace = enterPeekDirection.face;
      const Vec direction = groupSet->boundingSphere.center
        + (enterNormal * (float)SUBPARCEL_SIZE/2.0f)
        - position;
      if (direction.dot(enterNormal) <= 0) {
        for (const PeekDirection &exitPeekDirection : PEEK_DIRECTIONS) {
          const Vec &exitNormal = exitPeekDirection.normal;
          const PEEK_FACES &exitFace = exitPeekDirection.face;
          const int *exitINormal = exitPeekDirection.inormal;
          const Vec direction = groupSet->boundingSphere.center
            + (exitNormal * (float)SUBPARCEL_SIZE/2.0f)
            - position;
          if (direction.dot(exitNormal) >= 0 && groupSet->peeks[PEEK_FACE_INDICES[(int)enterFace << 4 | (int)exitFace]]) {
            int index = getSubparcelIndex(groupSet->x + exitINormal[0], groupSet->y + exitINormal[1], groupSet->z + exitINormal[2]);
            auto nextGroupSetIter = std::find_if(frustumGroupSets.begin(), frustumGroupSets.end(), [&](GroupSet *groupSet) -> bool {
              return groupSet->index == index;
            });
            if (nextGroupSetIter != frustumGroupSets.end()) {
              GroupSet *nextGroupSet = *nextGroupSetIter;
              if (nextGroupSet != nullptr && seenQueue.find(nextGroupSet) == seenQueue.end()) {
                queue.push_back(nextGroupSet);
                seenQueue.insert(nextGroupSet);
              }
            }
          }
        }
      }
    }
  }
  std::sort(cullResults, cullResults + numCullResults, [&](const CullResult &a, const CullResult &b) -> bool {
    return a.materialIndex < b.materialIndex;
  });
}