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
    bool _isDest;
    bool _isReached;
    float _priority;
    float _costSoFar;
    Voxel *_prev;
    Voxel *_next;
    Voxel *_leftVoxel;
    Voxel *_rightVoxel;
    Voxel *_btmVoxel;
    Voxel *_topVoxel;
    bool _canLeft;
    bool _canRight;
    bool _canBtm;
    bool _canTop;
    bool _isPath;
    bool _isFrontier;
  };
  void init(std::vector<PxRigidActor *> _actors, float hx, float hy, float hz, float *_position, float *_quaternion, float *_meshPosition, float *_meshQuaternion, unsigned int _maxIterDetect, unsigned int _numIgnorePhysicsIds, unsigned int *_ignorePhysicsIds);
  std::vector<Voxel> getPath(Vec start, Vec dest);
}

#endif