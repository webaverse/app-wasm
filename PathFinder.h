#include "physics.h"

using namespace physx;

namespace PathFinder {
  void detectPathVoxelStep(std::vector<PxRigidActor *> actors, PxGeometry *geom, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, int detectDir, unsigned int *iter, unsigned int maxIter, unsigned int numIgnorePhysicsIds, unsigned int *ignorePhysicsIds);
}