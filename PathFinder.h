#ifndef _PATHFINDER_H
#define _PATHFINDER_H

#include "physics.h"
#include <vector>

using namespace physx;

namespace PathFinder {
  struct Voxel {
    Vec position;
    bool _isStart = false;
    bool _isDest = false;
    bool _isReached = false;
    float _priority = 0;
    float _costSoFar = 0;
    Voxel *_prev = NULL;
    Voxel *_next = NULL;
    Voxel *_leftVoxel = NULL;
    Voxel *_rightVoxel = NULL;
    Voxel *_btmVoxel = NULL;
    Voxel *_topVoxel = NULL;
    Voxel *_backVoxel = NULL;
    Voxel *_frontVoxel = NULL;
    bool _canLeft = false;
    bool _canRight = false;
    bool _canBtm = false;
    bool _canTop = false;
    bool _canBack = false;
    bool _canFront = false;
    bool _isPath = false;
    bool _isFrontier = false;
  };
  void init(std::vector<PxRigidActor *> _actors, float _hy, float _heightTolerance, unsigned int _detectStep, unsigned int _maxIterdetect, unsigned int _maxIterStep, unsigned int _maxVoxelCacheLen, unsigned int _numIgnorePhysicsIds, unsigned int *_ignorePhysicsIds);
  std::vector<Voxel *> getPath(Vec start, Vec dest);
  std::vector<Voxel *> getVoxels();
}

#endif