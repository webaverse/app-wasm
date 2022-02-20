#ifndef _PATHFINDER_H
#define _PATHFINDER_H

#include "physics.h"
#include <vector>

using namespace physx;

namespace PathFinder {
  struct Voxel {
    float position[3];
  };
  std::vector<Voxel> detectPathVoxelStep(std::vector<PxRigidActor *> actors, PxGeometry *geom, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, int detectDir, unsigned int *iter, unsigned int maxIter, unsigned int numIgnorePhysicsIds, unsigned int *ignorePhysicsIds);
}

#endif