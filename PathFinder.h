#ifndef _PATHFINDER_H
#define _PATHFINDER_H

#include "physics.h"
#include <vector>

using namespace physx;

namespace PathFinder {
	// struct Vector3 {
	// 	float x; float y; float z;
	// };
  struct Voxel {
    Vec position;
    bool _isStart;
    bool _isReached;
    float _priority;
  };
  void init(std::vector<PxRigidActor *> _actors, float hx, float hy, float hz, float *_position, float *_quaternion, float *_meshPosition, float *_meshQuaternion, unsigned int _maxIterDetect, unsigned int _numIgnorePhysicsIds, unsigned int *_ignorePhysicsIds);
  std::vector<Voxel> getPath(Vec start, Vec dest);
}

#endif