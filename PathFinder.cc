#include <math.h>
#include <vector>
#include <map>
#include <string>

#include "PathFinder.h"

using namespace physx;

namespace PathFinder {

std::vector<Voxel> voxels;

std::vector<Voxel> detectPathVoxelStep(std::vector<PxRigidActor *> actors, PxGeometry *geom, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, int detectDir, unsigned int *iter, unsigned int maxIter, unsigned int numIgnorePhysicsIds, unsigned int *ignorePhysicsIds) {

  Voxel voxelA;
  voxelA.position[0] = 1;
  voxelA.position[1] = 3;
  voxelA.position[2] = 5;
  voxels.push_back(voxelA);

  Voxel voxelB;
  voxelB.position[0] = 2;
  voxelB.position[1] = 4;
  voxelB.position[2] = 6;

  if (*iter >= maxIter) return voxels;
  *iter = *iter + 1;

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
  
  float outY;
  bool anyHadHit = false;
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
          const unsigned int id = (unsigned int)actor->userData;
          
          bool includedInIgnores = false;
          for (int i = 0; i < numIgnorePhysicsIds; i++) {
            if (ignorePhysicsIds[i] == id) {
              includedInIgnores = true;
              break;
            }
          }

          if (!includedInIgnores) {
          // if ( id != 5) {
          // if ( id != ignorePhysicsIds[0]) {
            anyHadHit = true;
            break;
          }
        }
      }
    }
  }
  if (detectDir == 0) {
    if (anyHadHit) {
      detectDir = 1;
    } else {
      detectDir = -1;
    }
  }

  float detectStep = 0.1;
  if (detectDir == 1) {
    if (anyHadHit) {
      position[1] += detectDir * detectStep;
      detectPathVoxelStep(actors, geom, position, quaternion, meshPosition, meshQuaternion, detectDir, iter, maxIter, numIgnorePhysicsIds, ignorePhysicsIds);
    } else {
      // do nothing, stop recur
    }
  } else if (detectDir == -1) {
    if (anyHadHit) {
      position[1] += detectStep;
      // do nothing, stop recur
    } else {
      position[1] += detectDir * detectStep;
      detectPathVoxelStep(actors, geom, position, quaternion, meshPosition, meshQuaternion, detectDir, iter, maxIter, numIgnorePhysicsIds, ignorePhysicsIds);
    }
  }

  return voxels;
}

}