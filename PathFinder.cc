#include <math.h>
#include <vector>
#include <map>
#include <string>

#include "PathFinder.h"

using namespace physx;

namespace PathFinder {

std::vector<PxRigidActor *> actors;

float voxelHeight = 1.5;
float voxelHeightHalf = voxelHeight / 2;
float heightTolerance = 0.6;
Vec start;
Vec dest;
float detectStep = 0.1;
unsigned int iterDetect = 0;
unsigned int maxIterDetect = 1000;
unsigned int iterStep = 0;
unsigned int maxIterStep = 1000;
bool allowNearest = true;
bool isFound = false;
std::vector<Voxel> frontiers;
std::vector<Voxel> voxels;
std::vector<Voxel> waypointResult;
Voxel localVoxel;
PxBoxGeometry geom;
float position[3];
float quaternion[4];
float meshPosition[3];
float meshQuaternion[4];
unsigned int const numIgnorePhysicsIds = 1;
unsigned int ignorePhysicsIds[numIgnorePhysicsIds];

void init(std::vector<PxRigidActor *> _actors, float hx, float hy, float hz, float *_position, float *_quaternion, float *_meshPosition, float *_meshQuaternion, unsigned int _maxIterDetect, unsigned int _numIgnorePhysicsIds, unsigned int *_ignorePhysicsIds) {
  actors = _actors;

  PxBoxGeometry _geom(hx, hy, hz);
  geom = _geom;

  position[0] = _position[0];
  position[1] = _position[1];
  position[2] = _position[2];

  quaternion[0] = _quaternion[0];
  quaternion[1] = _quaternion[1];
  quaternion[2] = _quaternion[2];
  quaternion[3] = _quaternion[3];

  ignorePhysicsIds[0] = _ignorePhysicsIds[0];
}

void reset() {
  isFound = false;
  frontiers.clear();
  waypointResult.clear();
  voxels.clear();
}

void detect(Voxel *voxel, int detectDir) {

  if (iterDetect >= maxIterDetect) return;
  iterDetect = iterDetect + 1;

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
        bool result = PxGeometryQuery::overlap(geom, geomPose, geometry, meshPose3);
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
      (*voxel).position.y += detectDir * detectStep;
      detect(voxel, detectDir);
    } else {
      // do nothing, stop recur
    }
  } else if (detectDir == -1) {
    if (anyHadHit) {
      (*voxel).position.y += detectStep;
      // do nothing, stop recur
    } else {
      (*voxel).position.y += detectDir * detectStep;
      detect(voxel, detectDir);
    }
  }
}

Voxel createVoxel(Vec position) {
  localVoxel.position = position;
  localVoxel.position.y = std::round(localVoxel.position.y * 10) / 10; // Round position.y to 0.1 because detectStep is 0.1; // Need round both input and output of `detect()`, because of float calc precision problem. // TODO: Does cpp has precision problem too?
  iterDetect = 0;
  
  // PathFinder::init(actors);
  // std::vector<PathFinder::Voxel> voxels = PathFinder::detectPathVoxelStep(&geom, position, quaternion, meshPosition, meshQuaternion, 0, numIgnorePhysicsIds);

  return localVoxel;
}

std::vector<Voxel> getPath(Vec _start, Vec _dest) {
  reset();

  start.x = std::round(_start.x);
  start.y = _start.y;
  start.z = std::round(_start.z);
  dest.x = std::round(_dest.x);
  dest.y = _dest.y;
  dest.z = std::round(_dest.z);

  waypointResult.push_back(Voxel());
  waypointResult[0].position = start;
  waypointResult.push_back(Voxel());
  waypointResult[1].position = dest;

  return waypointResult;
}

}