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
unsigned int maxIterDetect = 2000;
unsigned int iterStep = 0;
unsigned int maxIterStep = 1000;
bool allowNearest = true;
unsigned int maxVoxelCacheLen = 10000;
bool isFound = false;
std::vector<Voxel *> frontiers;
std::vector<Voxel *> voxels;
std::vector<Voxel *> waypointResult;
std::map<std::string, Voxel *> voxelo;
Voxel localVoxel;
Voxel *startVoxel;
Voxel *destVoxel;
PxBoxGeometry geom;
float position[3];
float quaternion[4];
float meshPosition[3];
float meshQuaternion[4];
unsigned int const numIgnorePhysicsIds = 1;
unsigned int ignorePhysicsIds[numIgnorePhysicsIds];

Vec localVector;
Vec localVector2;

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

  meshPosition[0] = _meshPosition[0];
  meshPosition[1] = _meshPosition[1];
  meshPosition[2] = _meshPosition[2];

  meshQuaternion[0] = _meshQuaternion[0];
  meshQuaternion[1] = _meshQuaternion[1];
  meshQuaternion[2] = _meshQuaternion[2];
  meshQuaternion[3] = _meshQuaternion[3];

  ignorePhysicsIds[0] = _ignorePhysicsIds[0];
}

void resetVoxelAStar(Voxel *voxel) {
  voxel->_isStart = false;
  voxel->_isDest = false;
  voxel->_isReached = false;
  voxel->_priority = 0;
  voxel->_costSoFar = 0;
  voxel->_prev = NULL;
  voxel->_next = NULL;
  voxel->_isPath = false;
  voxel->_isFrontier = false;
}

void reset() {
  isFound = false;
  frontiers.clear();
  waypointResult.clear();

  // // pure realtime, no any cache
  // voxels.clear();
  // voxelo.clear();

  // simple cache
  int len = voxels.size();
  for (int i = 0; i < len; i++) {
    resetVoxelAStar(voxels[i]);
  }
}

void interpoWaypointResult() {
  Voxel *tempResult = waypointResult[0];
  waypointResult.erase(waypointResult.begin());
  localVector = tempResult->position;
  while (tempResult->_next) {
    localVector2 = tempResult->_next->position;

    tempResult->_next->position.x += localVector.x;
    tempResult->_next->position.x /= 2;
    tempResult->_next->position.y += localVector.y;
    tempResult->_next->position.y /= 2;
    tempResult->_next->position.z += localVector.z;
    tempResult->_next->position.z /= 2;

    tempResult = tempResult->_next;
    localVector = localVector2;
  }
}

// https://stackoverflow.com/a/4609795/3596736
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// https://www.geeksforgeeks.org/how-to-find-index-of-a-given-element-in-a-vector-in-cpp/
int getIndex(std::vector<Voxel *> v, Voxel * K) { // TODO: Support general type.
    auto it = find(v.begin(), v.end(), K);
    if (it != v.end()) { // If element was found
        int index = it - v.begin();
        return index;
    }
    else {
        return -1;
    }
}

void simplifyWaypointResult(Voxel *result) {
  if (result && result->_next && result->_next->_next) {
    if (
      sgn(result->_next->_next->position.x - result->_next->position.x) == sgn(result->_next->position.x - result->position.x) &&
      sgn(result->_next->_next->position.z - result->_next->position.z) == sgn(result->_next->position.z - result->position.z)
    ) {
      // waypointResult.splice(waypointResult.indexOf(result->_next), 1);
      int index = getIndex(waypointResult, result->_next);
      waypointResult.erase(waypointResult.begin() + index);

      result->_next = result->_next->_next;
      result->_next->_prev = result;
      simplifyWaypointResult(result);
    } else {
      simplifyWaypointResult(result->_next);
    }
  }
}

Voxel *getVoxel(Vec position) {
  // return voxelo[`${position.x}_${position.y}_${position.z}`];
  return voxelo[std::to_string(position.x)+"_"+std::to_string(position.y)+"_"+std::to_string(position.z)];
}

void setVoxelo(Voxel *voxel) {
  // voxelo[`${voxel.position.x}_${voxel.position.y}_${voxel.position.z}`] = voxel;
  voxelo[std::to_string(voxel->position.x)+"_"+std::to_string(voxel->position.y)+"_"+std::to_string(voxel->position.z)] = voxel;
}

void detect(Voxel *voxel, int detectDir) {

  if (iterDetect >= maxIterDetect) return;
  iterDetect = iterDetect + 1;

  PxTransform geomPose(
    PxVec3{voxel->position.x, voxel->position.y, voxel->position.z},
    PxQuat{quaternion[0], quaternion[1], quaternion[2], quaternion[3]}
  );
  PxTransform meshPose{
    PxVec3{meshPosition[0], meshPosition[1], meshPosition[2]},
    PxQuat{meshQuaternion[0], meshQuaternion[1], meshQuaternion[2], meshQuaternion[3]}
  };

  Vec p(meshPosition[0], meshPosition[1], meshPosition[2]);
  Quat q(meshQuaternion[0], meshQuaternion[1], meshQuaternion[2], meshQuaternion[3]);
  
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
      voxel->position.y += detectDir * detectStep;
      detect(voxel, detectDir);
    } else {
      // do nothing, stop recur
    }
  } else if (detectDir == -1) {
    if (anyHadHit) {
      voxel->position.y += detectStep;
      // do nothing, stop recur
    } else {
      voxel->position.y += detectDir * detectStep;
      detect(voxel, detectDir);
    }
  }
}

Voxel *createVoxel(Vec position) { // TODO: Use pointer?
  localVoxel.position = position;
  localVoxel.position.y = std::round(localVoxel.position.y * 10) / 10; // Round position.y to 0.1 because detectStep is 0.1; // Need round both input and output of `detect()`, because of float calc precision problem. // TODO: Does cpp has precision problem too?
  iterDetect = 0;
  detect(&localVoxel, 0);
  localVoxel.position.y = std::round(localVoxel.position.y * 10) / 10; // Round position.y to 0.1 because detectStep is 0.1; // Need round both input and output of `detect()`, because of float calc precision problem. // TODO: Does cpp has precision problem too?

  Voxel *existingVoxel = getVoxel(localVoxel.position);
  if(existingVoxel) return existingVoxel;

  Voxel *voxel = (Voxel *)malloc(sizeof(Voxel)); // https://stackoverflow.com/a/18041130/3596736
  *voxel = Voxel();
  voxels.push_back(voxel);
  resetVoxelAStar(voxel);

  voxel->position = localVoxel.position;
  setVoxelo(voxel);

  return voxel;
}

void setNextOfPathVoxel(Voxel *voxel) {
  if (voxel != NULL) {
    voxel->_isPath = true;
    if (voxel->_prev) voxel->_prev->_next = voxel;

    setNextOfPathVoxel(voxel->_prev);
  }
}

void found(Voxel *voxel) {
  isFound = true;
  setNextOfPathVoxel(voxel);

  Voxel wayPoint = *startVoxel; // wayPoint: voxel
  Voxel *result = (Voxel *)malloc(sizeof(Voxel)); // https://stackoverflow.com/a/18041130/3596736
  *result = wayPoint; // TODO: Directly = *startVoxel;
  waypointResult.push_back(result);
  while (wayPoint._next) {
    wayPoint = *wayPoint._next;

    result->_next = (Voxel *)malloc(sizeof(Voxel)); // https://stackoverflow.com/a/18041130/3596736
    *result->_next = wayPoint;
    waypointResult.push_back(result->_next);

    result->_next->_prev = result;

    result = result->_next;
  }
}

void generateVoxelMapLeft(Voxel *currentVoxel) {
  localVector = currentVoxel->position;
  localVector.x += -1;
  Voxel *leftVoxel = createVoxel(localVector);
  currentVoxel->_leftVoxel = leftVoxel;
  if (leftVoxel->position.y - currentVoxel->position.y < heightTolerance) {
    currentVoxel->_canLeft = true;
  }
}

void generateVoxelMapRight(Voxel *currentVoxel) {
  localVector = currentVoxel->position;
  localVector.x += 1;
  Voxel *rightVoxel = createVoxel(localVector);
  currentVoxel->_rightVoxel = rightVoxel;
  if (rightVoxel->position.y - currentVoxel->position.y < heightTolerance) {
    currentVoxel->_canRight = true;
  }
}

void generateVoxelMapBtm(Voxel *currentVoxel) {
  localVector = currentVoxel->position;
  localVector.z += -1;
  Voxel *btmVoxel = createVoxel(localVector);
  currentVoxel->_btmVoxel = btmVoxel;
  if (btmVoxel->position.y - currentVoxel->position.y < heightTolerance) {
    currentVoxel->_canBtm = true;
  }
}

void generateVoxelMapTop(Voxel *currentVoxel) {
  localVector = currentVoxel->position;
  localVector.z += 1;
  Voxel *topVoxel = createVoxel(localVector);
  currentVoxel->_topVoxel = topVoxel;
  if (topVoxel->position.y - currentVoxel->position.y < heightTolerance) {
    currentVoxel->_canTop = true;
  }
}

bool compareVoxelPriority(Voxel *a, Voxel *b) {
  return (a->_priority < b->_priority);
}

void stepVoxel(Voxel *voxel, Voxel *prevVoxel) {
  float newCost = prevVoxel->_costSoFar + 1;
  // if (voxel->_isReached === false || newCost < voxel->_costSoFar) {
  if (voxel->_isReached == false) {
    // Seems no need `|| newCost < voxel->_costSoFar` ? Need? http://disq.us/p/2mgpazs
    voxel->_isReached = true;
    voxel->_costSoFar = newCost;

    // voxel->_priority = tmpVec2.set(voxel->_x, voxel->_z).manhattanDistanceTo(dest)
    // voxel->_priority = tmpVec2.set(voxel->_x, voxel->_z).distanceToSquared(dest)
    voxel->_priority = voxel->position.distanceTo(dest);
    voxel->_priority += newCost;
    frontiers.push_back(voxel);
    // frontiers.sort((a, b) => a._priority - b._priority);
    sort(frontiers.begin(), frontiers.end(), compareVoxelPriority);

    voxel->_isFrontier = true;
    voxel->_prev = prevVoxel;
    // prevVoxel._next = voxel; // Can't assign _next here, because one voxel will has multiple _next. Need use `setNextOfPathVoxel()`.

    // if (voxel->_isDest) {
    // if (voxel->position.distanceTo(dest) == 0) { // TODO: PERFORMANCE: distanceToSq, or overload ==, or compare separately.
    if(voxel->position.x == dest.x && voxel->position.z == dest.z) { // TEST
      found(voxel);
    }
  }
}

void step() {
  if (frontiers.size() <= 0) {
    // if (debugRender) console.log('finish');
    return;
  }
  if (isFound) return;

  Voxel *currentVoxel = frontiers[0];
  frontiers.erase(frontiers.begin()); // shift
  currentVoxel->_isFrontier = false;

  if (!currentVoxel->_leftVoxel) generateVoxelMapLeft(currentVoxel);
  if (currentVoxel->_canLeft) {
    stepVoxel(currentVoxel->_leftVoxel, currentVoxel);
    if (isFound) return;
  }

  if (!currentVoxel->_rightVoxel) generateVoxelMapRight(currentVoxel);
  if (currentVoxel->_canRight) {
    stepVoxel(currentVoxel->_rightVoxel, currentVoxel);
    if (isFound) return;
  }

  if (!currentVoxel->_btmVoxel) generateVoxelMapBtm(currentVoxel);
  if (currentVoxel->_canBtm) {
    stepVoxel(currentVoxel->_btmVoxel, currentVoxel);
    if (isFound) return;
  }

  if (!currentVoxel->_topVoxel) generateVoxelMapTop(currentVoxel);
  if (currentVoxel->_canTop) {
    stepVoxel(currentVoxel->_topVoxel, currentVoxel);
    // if (isFound) return
  }
}

void untilFound() {
  iterStep = 0;
  while (frontiers.size() > 0 && !isFound) {
    if (iterStep >= maxIterStep) {
      // console.log('maxIterDetect: untilFound');

      if (allowNearest) {
        float minDistanceSquared = std::numeric_limits<float>::infinity();
        Voxel *minDistanceSquaredFrontier;
        int len = frontiers.size();
        for (int i = 0; i < len; i++) {
          Voxel *frontier = frontiers[i];
          float distanceSquared = frontier->position.distanceToSq(dest);
          if (distanceSquared < minDistanceSquared) {
            minDistanceSquared = distanceSquared;
            minDistanceSquaredFrontier = frontier;
          }
        }
        if (minDistanceSquaredFrontier) { // May all frontiers disappeared because of enclosed by obstacles, thus no minDistanceSquaredFrontier.
          found(minDistanceSquaredFrontier);
        }
      }

      return;
    }
    iterStep++;

    step();
  }
}

void disposeVoxelCache() {
  voxels.clear();
  voxelo.clear();
}

std::vector<Voxel *> getPath(Vec _start, Vec _dest) {
  reset();
  if (voxels.size() > maxVoxelCacheLen) disposeVoxelCache();

  start.x = std::round(_start.x);
  start.y = _start.y;
  start.z = std::round(_start.z);
  dest.x = std::round(_dest.x);
  dest.y = _dest.y;
  dest.z = std::round(_dest.z);

  startVoxel = createVoxel(start);
  startVoxel->_isStart = true;
  startVoxel->_isReached = true;
  startVoxel->_priority = start.distanceTo(dest);
  startVoxel->_costSoFar = 0;
  frontiers.push_back(startVoxel);

  destVoxel = createVoxel(dest);
  destVoxel->_isDest = true;

  // step();
  if (startVoxel == destVoxel) {
    found(destVoxel);
  } else {
    untilFound();
    if (isFound) {
      interpoWaypointResult();
      simplifyWaypointResult(waypointResult[0]);
      waypointResult.erase(waypointResult.begin()); // waypointResult.shift();
    }
    // console.log('waypointResult', waypointResult.length);
  }

  // waypointResult.push_back(destVoxel);
  // waypointResult.push_back(Voxel());
  // waypointResult[1].position = dest;

  // // TEST:
  // Voxel testVoxelA;
  // testVoxelA.position.x = frontiers.size();
  // testVoxelA.position.y = iterStep;
  // testVoxelA.position.z = waypointResult.size();
  // Voxel testVoxelB;
  // testVoxelB.position.x = startVoxel->_canTop;
  // testVoxelB.position.y = startVoxel->_btmVoxel == startVoxel->_topVoxel;
  // testVoxelB.position.z = frontiers.size();

  // waypointResult[0] = &testVoxelA;
  // waypointResult[1] = &testVoxelB;

  // waypointResult[2] = startVoxel->_btmVoxel;
  // waypointResult[3] = startVoxel->_topVoxel;


  return waypointResult;
  // return waypointResult.size();
  // return iterStep;
  // return isFound ? waypointResult : NULL;
}

}