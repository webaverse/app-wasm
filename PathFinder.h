#ifndef _PATHFINDER_H
#define _PATHFINDER_H

#include "physics.h"
#include <vector>

using namespace physx;

namespace PathFinder {
	struct Vector3 {
		float x; float y; float z;
	};
  struct Voxel {
    Vector3 position;
  };
  std::vector<Voxel> getPath(Vector3 start, Vector3 dest);
  std::vector<Voxel> detectPathVoxelStep(std::vector<PxRigidActor *> actors, PxGeometry *geom, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, int detectDir, unsigned int *iter, unsigned int maxIter, unsigned int numIgnorePhysicsIds, unsigned int *ignorePhysicsIds);
}

#endif