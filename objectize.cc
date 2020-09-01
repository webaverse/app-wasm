#include <emscripten.h>
#include "geometry.h"
#include "compose.h"
#include "noise.h"
#include "march.h"
#include "collide.h"
#include <earcut.hpp>
#include <RectBinPack/MaxRects.hpp>
// #include <iostream>

#include <deque>
#include <map>

extern "C" {

// memory

EMSCRIPTEN_KEEPALIVE void *doMalloc(size_t size) {
  return malloc(size);
}

EMSCRIPTEN_KEEPALIVE void doFree(void *ptr) {
  free(ptr);
}

EMSCRIPTEN_KEEPALIVE ArenaAllocator *makeArenaAllocator(unsigned int size) {
  return new ArenaAllocator(size);
}

/* EMSCRIPTEN_KEEPALIVE FreeEntry *arenaAlloc(ArenaAllocator *arenaAllocator, unsigned int size) {
  return arenaAllocator->alloc(size);
}

EMSCRIPTEN_KEEPALIVE void arenaFree(ArenaAllocator *arenaAllocator, FreeEntry *entry) {
  arenaAllocator->free(entry);
} */

// vegetation

EMSCRIPTEN_KEEPALIVE GeometrySet *makeGeometrySet() {
  return doMakeGeometrySet();
}

EMSCRIPTEN_KEEPALIVE void loadBake(GeometrySet *geometrySet, unsigned char *data, unsigned int size) {
  doLoadBake(geometrySet, data, size);
}

EMSCRIPTEN_KEEPALIVE void getGeometry(GeometrySet *geometrySet, char *nameData, unsigned int nameSize, float **positions, float **uvs, unsigned int **indices, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIndices) {
  doGetGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, *numPositions, *numUvs, *numIndices);
}

EMSCRIPTEN_KEEPALIVE void getAnimalGeometry(GeometrySet *geometrySet, unsigned int hash, float **positions, unsigned char **colors, unsigned int **indices, float **heads, float **legs, unsigned int *numPositions, unsigned int *numColors, unsigned int *numIndices, unsigned int *numHeads, unsigned int *numLegs, float *headPivot, float *aabb) {
  doGetAnimalGeometry(geometrySet, hash, positions, colors, indices, heads, legs, *numPositions, *numColors, *numIndices, *numHeads, *numLegs, headPivot, aabb);
}

/* EMSCRIPTEN_KEEPALIVE void getMarchObjectStats(GeometrySet *geometrySet, Subparcel *subparcel, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIds, unsigned int *numIndices, unsigned int *numSkyLights, unsigned int *numTorchLights) {
  doGetMarchObjectStats(geometrySet, subparcel, *numPositions, *numUvs, *numIds, *numIndices, *numSkyLights, *numTorchLights);
}

EMSCRIPTEN_KEEPALIVE void marchObjects(GeometrySet *geometrySet, int x, int y, int z, Subparcel *subparcel, Subparcel *subparcels, unsigned int numSubparcels, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int indexOffset) {
  doMarchObjects(geometrySet, x, y, z, subparcel, subparcels, numSubparcels, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);
} */

// land

EMSCRIPTEN_KEEPALIVE float doGetHeight(int seed, float ax, float ay, float az, float baseHeight) {
  return getHeight(seed, ax, ay, az, baseHeight);
}

/* EMSCRIPTEN_KEEPALIVE void doNoise3(int seed, int x, int y, int z, float baseHeight, float wormRate, float wormRadiusBase, float wormRadiusRate, float objectsRate, float potentialDefault, void *subparcelByteOffset) {
  noise3(seed, x, y, z, baseHeight, wormRate, wormRadiusBase, wormRadiusRate, objectsRate, potentialDefault, subparcelByteOffset);
}

EMSCRIPTEN_KEEPALIVE void doMarchingCubes2(float meshId, int dims[3], float *potential, unsigned char *biomes, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float *positions, float *normals, float *uvs, unsigned char *aos, float *ids, unsigned char *skyLights, unsigned char *torchLights, unsigned int *positionIndex, unsigned int *normalIndex, unsigned int *uvIndex, unsigned int *aoIndex, unsigned int *idIndex, unsigned int *skyLightsIndex, unsigned int *torchLightsIndex, unsigned int &numOpaquePositions, unsigned int &numTransparentPositions, unsigned char *peeks) {
  marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions, normals, uvs, aos, ids, skyLights, torchLights, *positionIndex, *normalIndex, *uvIndex, *aoIndex, *idIndex, *skyLightsIndex, *torchLightsIndex, numOpaquePositions, numTransparentPositions, peeks);
} */

// physics

/* EMSCRIPTEN_KEEPALIVE void initPhysx() {
  doInitPhysx();
}
EMSCRIPTEN_KEEPALIVE void registerBakedGeometry(unsigned int meshId, PxDefaultMemoryOutputStream *writeStream, float *meshPosition, float *meshQuaternion, uintptr_t *result) {
  *result = (uintptr_t)doRegisterBakedGeometry(meshId, writeStream, meshPosition, meshQuaternion);
}
EMSCRIPTEN_KEEPALIVE void registerBoxGeometry(unsigned int meshId, float *position, float *quaternion, float w, float h, float d, uintptr_t *result) {
  *result = (uintptr_t)doRegisterBoxGeometry(meshId, position, quaternion, w, h, d);
}
EMSCRIPTEN_KEEPALIVE void registerCapsuleGeometry(unsigned int meshId, float *position, float *quaternion, float radius, float halfHeight, uintptr_t *result) {
  *result = (uintptr_t)doRegisterCapsuleGeometry(meshId, position, quaternion, radius, halfHeight);
} */
EMSCRIPTEN_KEEPALIVE PxDefaultMemoryOutputStream *bakeGeometry(Physicer *physicer, float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices) {
  return doBakeGeometry(physicer, positions, indices, numPositions, numIndices);
}
EMSCRIPTEN_KEEPALIVE PxDefaultMemoryOutputStream *bakeConvexGeometry(Physicer *physicer, float *positions, unsigned int *indices, unsigned int numPositions, unsigned int numIndices) {
  return doBakeConvexGeometry(physicer, positions, indices, numPositions, numIndices);
}
/* EMSCRIPTEN_KEEPALIVE void releaseBakedGeometry(PxDefaultMemoryOutputStream *writeStream) {
  delete writeStream;
}
EMSCRIPTEN_KEEPALIVE void unregisterGeometry(GeometrySpec *geometrySpec) {
  doUnregisterGeometry(geometrySpec);
} */
EMSCRIPTEN_KEEPALIVE void raycast(Tracker *tracker, float *origin, float *direction, float *meshPosition, float *meshQuaternion, unsigned int *hit, float *position, float *normal, float *distance, unsigned int *objectId, Vec *outPosition, Quat *outQuaternion) {
  doRaycast(tracker, origin, direction, meshPosition, meshQuaternion, *hit, position, normal, *distance, *objectId, *outPosition, *outQuaternion);
}
EMSCRIPTEN_KEEPALIVE void collide(Tracker *tracker, float radius, float halfHeight, float *position, float *quaternion, float *meshPosition, float *meshQuaternion, unsigned int maxIter, unsigned int *hit, float *direction, unsigned int *grounded) {
  doCollide(tracker, radius, halfHeight, position, quaternion, meshPosition, meshQuaternion, maxIter, *hit, direction, *grounded);
}

// culling

/* EMSCRIPTEN_KEEPALIVE void *makeCuller() {
  return doMakeCuller();
}
EMSCRIPTEN_KEEPALIVE GroupSet *registerGroupSet(Culler *culler, int x, int y, int z, float r, unsigned char *peeks, Group *groups, unsigned int numGroups) {
  return doRegisterGroupSet(culler, x, y, z, r, peeks, groups, numGroups);
}
EMSCRIPTEN_KEEPALIVE void unregisterGroupSet(Culler *culler, GroupSet *groupSet) {
  doUnregisterGroupSet(culler, groupSet);
}
EMSCRIPTEN_KEEPALIVE void cull(Culler *culler, float *positionData, float *matrixData, float slabRadius, CullResult *cullResults, unsigned int *numCullResults) {
  doCull(culler, positionData, matrixData, slabRadius, cullResults, *numCullResults);
} */
EMSCRIPTEN_KEEPALIVE void tickCull(Tracker *tracker, float *positionData, float *matrixData, CullResult *landCullResults, unsigned int &numLandCullResults, CullResult *vegetationCullResults, unsigned int &numVegetationCullResults) {
  doTickCull(tracker, positionData, matrixData, landCullResults, numLandCullResults, vegetationCullResults, numVegetationCullResults);
}

// tracking

EMSCRIPTEN_KEEPALIVE Tracker *makeTracker(
  int seed,
  unsigned int meshId,
  int chunkDistance,

  ArenaAllocator *landPositionsAllocator,
  ArenaAllocator *landNormalsAllocator,
  ArenaAllocator *landUvsAllocator,
  // ArenaAllocator *landBarycentricsAllocator,
  ArenaAllocator *landAosAllocator,
  ArenaAllocator *landIdsAllocator,
  ArenaAllocator *landSkyLightsAllocator,
  ArenaAllocator *landTorchLightsAllocator,

  ArenaAllocator *vegetationPositionsAllocator,
  ArenaAllocator *vegetationUvsAllocator,
  ArenaAllocator *vegetationAtlasUvsAllocator,
  ArenaAllocator *vegetationIdsAllocator,
  ArenaAllocator *vegetationIndicesAllocator,
  ArenaAllocator *vegetationSkyLightsAllocator,
  ArenaAllocator *vegetationTorchLightsAllocator
) {
  return new Tracker(
    seed,
    meshId,
    chunkDistance,

    landPositionsAllocator,
    landNormalsAllocator,
    landUvsAllocator,
    // landBarycentricsAllocator,
    landAosAllocator,
    landIdsAllocator,
    landSkyLightsAllocator,
    landTorchLightsAllocator,

    vegetationPositionsAllocator,
    vegetationUvsAllocator,
    vegetationAtlasUvsAllocator,
    vegetationIdsAllocator,
    vegetationIndicesAllocator,
    vegetationSkyLightsAllocator,
    vegetationTorchLightsAllocator
  );
}

EMSCRIPTEN_KEEPALIVE void tickTracker(Tracker *tracker, ThreadPool *threadPool, GeometrySet *geometrySet, float x, float y, float z) {
  tracker->updateNeededCoords(threadPool, geometrySet, x, y, z);
}

EMSCRIPTEN_KEEPALIVE void doChunk(float meshId, int dims[3], float *potential, unsigned char *biomes, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float *positions, float *normals, float *uvs, /*float *barycentrics,*/ unsigned char *aos, float *ids, unsigned char *skyLights, unsigned char *torchLights, unsigned int *positionIndex, unsigned int *normalIndex, unsigned int *uvIndex, /*unsigned int *barycentricIndex,*/ unsigned int *aoIndex, unsigned int *idIndex, unsigned int *skyLightsIndex, unsigned int *torchLightsIndex, unsigned int &numOpaquePositions, unsigned int &numTransparentPositions, unsigned char *peeks) {
  marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions, normals, uvs, /*barycentrics,*/ aos, ids, skyLights, torchLights, *positionIndex, *normalIndex, *uvIndex, /**barycentricIndex,*/ *aoIndex, *idIndex, *skyLightsIndex, *torchLightsIndex, numOpaquePositions, numTransparentPositions, peeks);
}

// earcut

class EarcutResult {
public:
  float *positions;
  unsigned int numPositions;
  float *uvs;
  unsigned int numUvs;
  uint32_t *indices;
  unsigned int numIndices;
  /* std::shared_ptr<PhysicsGeometry> physicsGeometry;
  PhysicsGeometry *physicsGeometryPtr; */
};

struct CustomRect {
  CustomRect() : x(0), y(0), w(1), h(1), packed(false) {}
  CustomRect(float w, float h) : x(0), y(0), w(w), h(h), packed(false) {}
  float x, y, w, h;
  bool packed;
};
constexpr unsigned int size = 4096;
float sizeScale = 1;
inline RectBinPack::Rect toRect(const CustomRect& value) {
  return {
    (unsigned int)(value.x * (float)size),
    (unsigned int)(value.y * (float)size),
    (unsigned int)(value.w * (float)size * sizeScale),
    (unsigned int)(value.h * (float)size * sizeScale),
  };
}
inline void fromBinRect(CustomRect& value, RectBinPack::BinRect rect) {
  value.x = (float)rect.rect.x/((float)size);
  value.y = (float)rect.rect.y/((float)size);
  value.w = (float)rect.rect.width/((float)size);
  value.h = (float)rect.rect.height/((float)size);

  // If bin is not set, set rectangle to unpacked
  value.packed = rect.bin != RectBinPack::InvalidBin;
}
EMSCRIPTEN_KEEPALIVE EarcutResult *earcut(float *positions, unsigned int *counts, unsigned int numCounts, float *points, unsigned int numPoints, float z, float *zs) {
  std::vector<std::vector<std::array<float, 2>>> polygon;
  std::map<unsigned int, std::vector<unsigned int>> connectivity;
  std::vector<unsigned int> islandIndices;
  std::vector<float> islandVs;
  std::vector<float> islandHeights(numCounts + numPoints);
  std::vector<float> islandV(numCounts + numPoints);

  // add regular points
  unsigned int numCoreVertices = 0;
  unsigned int numVertices = 0;
  unsigned int numIndices = 0;
  {
    for (unsigned int i = 0; i < numCounts; i++) {
      unsigned int count = counts[i];
      std::vector<std::array<float, 2>> points;
      points.reserve(count);
      for (unsigned int j = 0; j < count; j++) {
        points.push_back(std::array<float, 2>{positions[numIndices*2], positions[numIndices*2+1]});

        unsigned int nextIndex = (j+1 < count) ? (numIndices+1) : (numIndices+1-count);
        connectivity[numIndices].push_back(nextIndex);
        connectivity[nextIndex].push_back(numIndices);

        islandIndices.push_back(i);
        islandVs.push_back(islandHeights[i]);
        float dx = positions[numIndices*2] - positions[nextIndex*2];
        float dy = positions[numIndices*2+1] - positions[nextIndex*2+1];
        islandHeights[i] += (float)std::sqrt(dx*dx + dy*dy);

        numCoreVertices += 3;
        numVertices += 3;
        numIndices++;
      }
      polygon.push_back(std::move(points));
    }
  }
  // add hole points
  {
    for (unsigned int i = 0; i < numPoints; i++) {
      float srcPoint[2] = {
        points[i*2],
        points[i*2+1],
      };
      std::vector<std::array<float, 2>> points{
        {
          srcPoint[0],
          srcPoint[1] + 0.01f,
        },
        {
          srcPoint[0] - 0.01f,
          srcPoint[1] - 0.01f,
        },
        {
          srcPoint[0] + 0.01f,
          srcPoint[1] - 0.01f,
        },
      };
      polygon.push_back(std::move(points));

      connectivity[numIndices] = {
        numIndices + 1,
        numIndices + 2,
      };
      connectivity[numIndices+1] = {
        numIndices,
        numIndices + 2,
      };
      connectivity[numIndices+2] = {
        numIndices,
        numIndices + 1,
      };

      islandIndices.push_back(numCounts + i);
      islandVs.push_back(islandHeights[numCounts + i]);
      islandHeights[numCounts + i] = 1;

      numVertices += 9;
      numIndices += 3;
    }
  }

  // Run tessellation
  // Returns array of indices that refer to the vertices of the input polygon.
  // e.g: the index 6 would refer to {25, 75} in this example.
  // Three subsequent indices form a triangle. Output triangles are clockwise.
  std::vector<uint32_t> *indicesPtr = new std::vector<uint32_t>();
  std::vector<uint32_t> &indices = *indicesPtr;
  mapbox::earcut<uint32_t>(polygon, indices);

  // compute flat min max
  float min[2] = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
  float max[2] = {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
  for (unsigned int i = 0; i < counts[0]; i += 3) {
    float x = positions[i];
    float y = positions[i+1];
    if (x < min[0]) min[0] = x;
    if (y < min[1]) min[1] = y;
    if (x > max[0]) max[0] = x;
    if (y > max[1]) max[1] = y;
  }
  float width = max[0] - min[0];
  float height = max[1] - min[1];

  // compute connections min max
  CustomRect flatRect = width < height ? CustomRect(width/height, 1) : CustomRect(1, height/width);
  std::vector<CustomRect> rects{
    flatRect,
    flatRect,
  };
  for (unsigned int i = 0; i < islandHeights.size(); i++) {
    float h = islandHeights[i];
    rects.push_back(h > 0 ? (z < h ? CustomRect(z/h, 1) : CustomRect(1, h/z)) : CustomRect(0, 0));
  }
  for (sizeScale = 1;; sizeScale /= 2.0f) {
    // Initialize configuration (size x size, 1 bin, no flipping, BestAreaFit)
    RectBinPack::MaxRectsConfiguration config{
      size, size, 1, 1, false, RectBinPack::MaxRectsHeuristic::BestAreaFit
    };
    RectBinPack::packMaxRects(config, rects);

    for (CustomRect &rect : rects) {
      // std::cout << "rect " << rect.x << " " << rect.y << " " << rect.w << " " << rect.h << " " << sizeScale << std::endl;
      if (!rect.packed) {
        // std::cout << "retry " << sizeScale << std::endl;
        continue; // retry
      }
    }
    break;
  }

  // collapse hole points
  {
    unsigned int offset = 0;
    for (unsigned int i = 0; i < numPoints; i++) {
      float srcPoint[2] = {
        points[i*2],
        points[i*2+1],
      };
      auto &line = polygon[polygon.size() - numPoints + i];
      for (auto &point : line) {
        point[0] = srcPoint[0];
        point[1] = srcPoint[1];
      }
    }
  }
  for (unsigned int i = 0; i < indices.size(); i++) {
    if (indices[i] > numCoreVertices/3) {
      indices[i] = (indices[i]/3)*3;
    }
  }

  std::vector<float> *outPositionsPtr = new std::vector<float>(numVertices*2);
  std::vector<float> &outPositions = *outPositionsPtr;
  std::vector<float> *uvsPtr = new std::vector<float>(outPositions.size());
  std::vector<float> &uvs = *uvsPtr;
  {
    unsigned int i = 0;
    float fi = 0;
    unsigned int index = 0;
    for (float dz = 0; dz <= z; dz += z, i++, fi++) {
      CustomRect &rect = rects[i];
      unsigned int index2 = 0;
      for (auto &line : polygon) {
        for (auto &point : line) {
          outPositions[index*3] = point[0];
          outPositions[index*3+1] = point[1];
          outPositions[index*3+2] = dz + zs[index2];

          float u = (point[0] - min[0])/width;
          float v = (point[1] - min[1])/height;
          uvs[index*3] = rect.x + u * rect.w;
          uvs[index*3+1] = rect.y + v * rect.h;
          uvs[index*3+2] = fi;

          index++;
          index2++;
        }
      }
    }
  }

  // std::list<std::set<unsigned int>> islands;
  if (z > 0) {
    // double-layer points
    unsigned int halfSize = indices.size();
    indices.resize(halfSize*2);
    for (unsigned int i = 0; i < halfSize; i++) {
      indices[halfSize + i] = indices[i] + numVertices/3;
    }

    // connect layers and deduplicate
    std::map<unsigned int, unsigned int> duplicatedIndices;
    for (unsigned int i = 0; i < halfSize; i += 3) {
      unsigned int ai = indices[i];
      unsigned int bi = indices[i+1];
      unsigned int ci = indices[i+2];
      unsigned int dai = indices[halfSize + i];
      unsigned int dbi = indices[halfSize + i+1];
      unsigned int dci = indices[halfSize + i+2];

      auto &aConnectivity = connectivity[ai];
      auto &bConnectivity = connectivity[bi];
      auto &cConnectivity = connectivity[ci];

      if (std::find(aConnectivity.begin(), aConnectivity.end(), bi) != aConnectivity.end()) {
        unsigned int islandIndex = islandIndices[ai];
        CustomRect &rect = rects[2 + islandIndex];

        unsigned int dupeAIndex;
        unsigned int dupeBIndex;
        unsigned int dupeDAIndex;
        unsigned int dupeDBIndex;
        auto dupeAIter = duplicatedIndices.find(ai);
        if (dupeAIter == duplicatedIndices.end()) {
          dupeAIndex = duplicatedIndices[ai] = outPositions.size()/3;
          outPositions.push_back(outPositions[ai*3]);
          outPositions.push_back(outPositions[ai*3+1]);
          outPositions.push_back(outPositions[ai*3+2]);

          uvs.push_back(rect.x);
          uvs.push_back(rect.y + islandVs[ai]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);

          dupeDAIndex = duplicatedIndices[dai] = outPositions.size()/3;
          outPositions.push_back(outPositions[dai*3]);
          outPositions.push_back(outPositions[dai*3+1]);
          outPositions.push_back(outPositions[dai*3+2]);

          uvs.push_back(rect.x + rect.w);
          uvs.push_back(rect.y + islandVs[ai]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);
        } else {
          dupeAIndex = dupeAIter->second;
          dupeDAIndex = duplicatedIndices[dai];
        }
        auto dupeBIter = duplicatedIndices.find(bi);
        if (dupeBIter == duplicatedIndices.end()) {
          dupeBIndex = duplicatedIndices[bi] = outPositions.size()/3;
          outPositions.push_back(outPositions[bi*3]);
          outPositions.push_back(outPositions[bi*3+1]);
          outPositions.push_back(outPositions[bi*3+2]);

          uvs.push_back(rect.x);
          uvs.push_back(rect.y + islandVs[bi]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);

          dupeDBIndex = duplicatedIndices[dbi] = outPositions.size()/3;
          outPositions.push_back(outPositions[dbi*3]);
          outPositions.push_back(outPositions[dbi*3+1]);
          outPositions.push_back(outPositions[dbi*3+2]);

          uvs.push_back(rect.x + rect.w);
          uvs.push_back(rect.y + islandVs[bi]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);
        } else {
          dupeBIndex = dupeBIter->second;
          dupeDBIndex = duplicatedIndices[dbi];
        }

        indices.push_back(dupeAIndex);
        indices.push_back(dupeBIndex);
        indices.push_back(dupeDAIndex);
        indices.push_back(dupeBIndex);
        indices.push_back(dupeDBIndex);
        indices.push_back(dupeDAIndex);
      }
      if (std::find(bConnectivity.begin(), bConnectivity.end(), ci) != bConnectivity.end()) {
        unsigned int islandIndex = islandIndices[bi];
        CustomRect &rect = rects[2 + islandIndex];

        unsigned int dupeBIndex;
        unsigned int dupeCIndex;
        unsigned int dupeDBIndex;
        unsigned int dupeDCIndex;
        auto dupeBIter = duplicatedIndices.find(bi);
        if (dupeBIter == duplicatedIndices.end()) {
          dupeBIndex = duplicatedIndices[bi] = outPositions.size()/3;
          outPositions.push_back(outPositions[bi*3]);
          outPositions.push_back(outPositions[bi*3+1]);
          outPositions.push_back(outPositions[bi*3+2]);

          uvs.push_back(rect.x);
          uvs.push_back(rect.y + islandVs[bi]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);

          dupeDBIndex = duplicatedIndices[dbi] = outPositions.size()/3;
          outPositions.push_back(outPositions[dbi*3]);
          outPositions.push_back(outPositions[dbi*3+1]);
          outPositions.push_back(outPositions[dbi*3+2]);

          uvs.push_back(rect.x + rect.w);
          uvs.push_back(rect.y + islandVs[bi]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);
        } else {
          dupeBIndex = dupeBIter->second;
          dupeDBIndex = duplicatedIndices[dbi];
        }
        auto dupeCIter = duplicatedIndices.find(ci);
        if (dupeCIter == duplicatedIndices.end()) {
          dupeCIndex = duplicatedIndices[ci] = outPositions.size()/3;
          outPositions.push_back(outPositions[ci*3]);
          outPositions.push_back(outPositions[ci*3+1]);
          outPositions.push_back(outPositions[ci*3+2]);

          uvs.push_back(rect.x);
          uvs.push_back(rect.y + islandVs[ci]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);

          dupeDCIndex = duplicatedIndices[dci] = outPositions.size()/3;
          outPositions.push_back(outPositions[dci*3]);
          outPositions.push_back(outPositions[dci*3+1]);
          outPositions.push_back(outPositions[dci*3+2]);

          uvs.push_back(rect.x + rect.w);
          uvs.push_back(rect.y + islandVs[ci]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);
        } else {
          dupeCIndex = dupeCIter->second;
          dupeDCIndex = duplicatedIndices[dci];
        }

        indices.push_back(dupeBIndex);
        indices.push_back(dupeCIndex);
        indices.push_back(dupeDBIndex);
        indices.push_back(dupeCIndex);
        indices.push_back(dupeDCIndex);
        indices.push_back(dupeDBIndex);
      }
      if (std::find(cConnectivity.begin(), cConnectivity.end(), ai) != cConnectivity.end()) {
        unsigned int islandIndex = islandIndices[ci];
        CustomRect &rect = rects[2 + islandIndex];

        unsigned int dupeCIndex;
        unsigned int dupeAIndex;
        unsigned int dupeDCIndex;
        unsigned int dupeDAIndex;
        auto dupeCIter = duplicatedIndices.find(bi);
        if (dupeCIter == duplicatedIndices.end()) {
          dupeCIndex = duplicatedIndices[ci] = outPositions.size()/3;
          outPositions.push_back(outPositions[ci*3]);
          outPositions.push_back(outPositions[ci*3+1]);
          outPositions.push_back(outPositions[ci*3+2]);

          uvs.push_back(rect.x);
          uvs.push_back(rect.y + islandVs[ci]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);

          dupeDCIndex = duplicatedIndices[dci] = outPositions.size()/3;
          outPositions.push_back(outPositions[dci*3]);
          outPositions.push_back(outPositions[dci*3+1]);
          outPositions.push_back(outPositions[dci*3+2]);

          uvs.push_back(rect.x + rect.w);
          uvs.push_back(rect.y + islandVs[ci]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);
        } else {
          dupeCIndex = dupeCIter->second;
          dupeDCIndex = duplicatedIndices[dci];
        }
        auto dupeAIter = duplicatedIndices.find(ai);
        if (dupeAIter == duplicatedIndices.end()) {
          dupeAIndex = duplicatedIndices[ai] = outPositions.size()/3;
          outPositions.push_back(outPositions[ai*3]);
          outPositions.push_back(outPositions[ai*3+1]);
          outPositions.push_back(outPositions[ai*3+2]);

          uvs.push_back(rect.x);
          uvs.push_back(rect.y + islandVs[ai]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);

          dupeDAIndex = duplicatedIndices[dai] = outPositions.size()/3;
          outPositions.push_back(outPositions[dai*3]);
          outPositions.push_back(outPositions[dai*3+1]);
          outPositions.push_back(outPositions[dai*3+2]);

          uvs.push_back(rect.x + rect.w);
          uvs.push_back(rect.y + islandVs[ai]/islandHeights[islandIndex] * rect.h);
          uvs.push_back((float)islandIndex);
        } else {
          dupeAIndex = dupeAIter->second;
          dupeDAIndex = duplicatedIndices[dai];
        }

        indices.push_back(dupeCIndex);
        indices.push_back(dupeAIndex);
        indices.push_back(dupeDCIndex);
        indices.push_back(dupeAIndex);
        indices.push_back(dupeDAIndex);
        indices.push_back(dupeDCIndex);
      }
    }

    /* // collect connection islands
    for (unsigned int i = halfSize*2; i < indices.size(); i += 3) {
      unsigned int a = indices[i];
      unsigned int b = indices[i+1];
      unsigned int c = indices[i+2];

      auto aIter = std::find_if(islands.begin(), islands.end(), [&](std::set<unsigned int> &island) -> bool {
        return island.find(a) != island.end();
      });
      auto bIter = std::find_if(islands.begin(), islands.end(), [&](std::set<unsigned int> &island) -> bool {
        return island.find(b) != island.end();
      });
      auto cIter = std::find_if(islands.begin(), islands.end(), [&](std::set<unsigned int> &island) -> bool {
        return island.find(c) != island.end();
      });
      decltype(aIter) iter;
      if (aIter != islands.end()) {
        iter = aIter;
      } else if (bIter != islands.end()) {
        iter = bIter;
      } else if (cIter != islands.end()) {
        iter = cIter;
      } else {
        iter = islands.end();
      }
      if (
        (aIter != islands.end())
      ) {
        if (bIter != islands.end() && bIter != aIter) {
          iter->insert(bIter->begin(), bIter->end());
          islands.erase(bIter);
          bIter = islands.end();
        }
        if (cIter != islands.end() && cIter != aIter) {
          iter->insert(cIter->begin(), cIter->end());
          islands.erase(cIter);
          cIter = islands.end();
        }
      }
      if (
        (bIter != islands.end())
      ) {
        if (aIter != islands.end() && aIter != bIter) {
          iter->insert(aIter->begin(), aIter->end());
          islands.erase(aIter);
          aIter = islands.end();
        }
        if (cIter != islands.end() && cIter != bIter) {
          iter->insert(cIter->begin(), cIter->end());
          islands.erase(cIter);
          cIter = islands.end();
        }
      }
      if (
        (cIter != islands.end())
      ) {
        if (aIter != islands.end() && aIter != cIter) {
          iter->insert(aIter->begin(), aIter->end());
          islands.erase(aIter);
          aIter = islands.end();
        }
        if (bIter != islands.end() && bIter != cIter) {
          iter->insert(bIter->begin(), bIter->end());
          islands.erase(bIter);
          bIter = islands.end();
        }
      }
      if (iter != islands.end()) {
        std::set<unsigned int> &island = *iter;
        island.insert(a);
        island.insert(b);
        island.insert(c);
      } else {
        std::set<unsigned int> island{a, b, c};
        islands.push_back(std::move(island));
      }
    } */
    // std::cout << "num islands " << islands.size() << std::endl;

    // flip back points
    for (unsigned int i = 0; i < halfSize; i += 3) {
      std::swap(indices[i + 1], indices[i + 2]);
    }
  }

  /* PxDefaultMemoryOutputStream *physicsGeometryDataStream = doBakeGeometry(&tracker->physicer, outPositions.data(), indices.data(), outPositions.size(), indices.size());
  float meshPosition[] = {0, 0, 0};
  float meshQuaternion[] = {0, 0, 0, 1};
  std::shared_ptr<PhysicsGeometry> physicsGeometry = doMakeBakedGeometry(&tracker->physicer, physicsGeometryDataStream, meshPosition, meshQuaternion);
  delete physicsGeometryDataStream; */

  EarcutResult *result = new EarcutResult();
  result->positions = outPositions.data();
  result->numPositions = outPositions.size();
  result->uvs = uvs.data();
  result->numUvs = uvs.size();
  result->indices = indices.data();
  result->numIndices = indices.size();
  /* result->physicsGeometry = std::move(physicsGeometry);
  result->physicsGeometryPtr = result->physicsGeometry.get(); */
  return result;
}

// requests

EMSCRIPTEN_KEEPALIVE ThreadPool *makeThreadPool(unsigned int numThreads) {
  return new ThreadPool(numThreads);
}

/* unsigned int nextMessageId = 0;
EMSCRIPTEN_KEEPALIVE unsigned int pushRequest(ThreadPool *threadPool, Message *message) {
  unsigned int messageId = ++nextMessageId;
  message->id = messageId;
  threadPool->inbox.queue(message);
  return messageId;
}

EMSCRIPTEN_KEEPALIVE unsigned int popResponse(ThreadPool *threadPool) {
  ResponseMessage *responseMessage = threadPool->outbox.pop();
  if (responseMessage) {
    unsigned int messageId = responseMessage->id;
    delete responseMessage;
    return messageId;
  } else {
    return 0;
  }
} */

EMSCRIPTEN_KEEPALIVE void tick(ThreadPool *threadPool, unsigned char *ptr, unsigned int numEntries, unsigned char *outPtr, unsigned int *outNumEntries) {
  {
    std::vector<Message *> outMessages;
    threadPool->outbox.popAll(outMessages);
    // std::cout << "out messages a " << outMessages.size() << std::endl;
    unsigned int index = 0;
    int i = 0;
    for (Message *outMessage : outMessages) {
      Message *clientMessage = (Message *)(outPtr + index);

      unsigned int offsetCount = outMessage->count;
      unsigned int offsetSize = sizeof(Message) - 4 + offsetCount*sizeof(unsigned int);
      memcpy(clientMessage, outMessage, offsetSize);

      // std::cout << "out id " << i << " " << outMessages.size() << " " << clientMessage->id << " " << index << " " << (unsigned int)clientMessage << std::endl;
      /* if (!clientMessage->id || clientMessage->id > 100) {
        std::cout << "no id a " << clientMessage->id << " " << index << std::endl;
        abort();
      } */

      index += offsetSize;

      free(outMessage);
      i++;
    }
    // std::cout << "out messages b " << outMessages.size() << std::endl;
    *outNumEntries = outMessages.size();
  }
  {
    Message *message;
    while ((message = threadPool->dependencyInbox.pop())) {
      message->priority = 1;
      threadPool->inbox.queue(message);
    }
  }
  {
    std::vector<Message *> inMessages(numEntries);
    unsigned int index = 0;
    for (unsigned int i = 0; i < numEntries; i++) {
      Message *clientMessage = (Message *)(ptr + index);

      unsigned int offsetCount = clientMessage->count;
      unsigned int offsetSize = sizeof(Message) - 4 + offsetCount*sizeof(unsigned int);

      Message *newMessage = (Message *)malloc(offsetSize);
      memcpy(newMessage, clientMessage, offsetSize);
      // std::cout << "in id " << newMessage->id << std::endl;
      /* if (!newMessage->id || newMessage->id > 100) {
        std::cout << "no id b " << newMessage->id << " " << index << std::endl;
        abort();
      } */

      inMessages[i] = newMessage;
      index += offsetSize;
    }
    threadPool->inbox.queueAll(std::move(inMessages));
  }
}

bool checkSubparcelIndicesLive(Tracker *tracker, const std::vector<int> indices) {
  std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
  for (int index : indices) {
    auto iter = tracker->subparcels.find(index);
    if (iter != tracker->subparcels.end()) {
      std::shared_ptr<Subparcel> &subparcel = iter->second;
      if (!subparcel->live) {
        return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

std::function<void(ThreadPool *, Message *)> METHOD_FNS[] = {
  [](ThreadPool *threadPool, Message *Message) -> void { // makeArenaAllocator
    abort();
    /* unsigned int index = 0;
    unsigned int size = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    ArenaAllocator **arenaAllocator = (ArenaAllocator **)(Message->args + index);
    index += sizeof(ArenaAllocator *);

    *arenaAllocator = makeArenaAllocator(size); v*/
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // arenaAlloc
    abort();
    /* unsigned int index = 0;
    ArenaAllocator *arenaAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    unsigned int size = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    FreeEntry **entry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry **);

    *entry = arenaAlloc(arenaAllocator, size); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // arenaFree
    abort();
    /* unsigned int index = 0;
    ArenaAllocator *arenaAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    FreeEntry *entry = *((FreeEntry **)(Message->args + index));
    index += sizeof(FreeEntry *);

    arenaFree(arenaAllocator, entry); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // makeGeometrySet
    abort();
    /* GeometrySet **geometrySet = (GeometrySet **)Message->args;
    *geometrySet = new GeometrySet(); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // loadBake
    GeometrySet *geometrySet = *((GeometrySet **)Message->args);
    unsigned char *data = *((unsigned char **)(Message->args + sizeof(GeometrySet *)));
    unsigned int dataSize = *((unsigned int *)(Message->args + sizeof(GeometrySet *) + sizeof(unsigned char *)));
    // std::cout << "load bake 1 " << (unsigned int)(void *)geometrySet << " " << (unsigned int)data << " " << dataSize << std::endl;
    loadBake(geometrySet, data, dataSize);
    // std::cout << "load bake 2" << std::endl;

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // getGeometry
    unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *nameData = *((char **)(Message->args + index));
    index += sizeof(char *);
    unsigned int nameSize = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(Message->args + index);
    index += sizeof(float **);
    float **uvs = (float **)(Message->args + index);
    index += sizeof(float **);
    unsigned int **indices = (unsigned int **)(Message->args + index);
    index += sizeof(unsigned int **);
    unsigned int *numPositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numUvs = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);

    getGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, numPositions, numUvs, numIndices);

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // getAnimalGeometry
    unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    unsigned int hash = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(Message->args + index);
    index += sizeof(float **);
    unsigned char **colors = (unsigned char **)(Message->args + index);
    index += sizeof(unsigned char **);
    unsigned int **indices = (unsigned int **)(Message->args + index);
    index += sizeof(unsigned int **);
    float **heads = (float **)(Message->args + index);
    index += sizeof(float **);
    float **legs = (float **)(Message->args + index);
    index += sizeof(float **);
    unsigned int *numPositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numColors = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numHeads = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numLegs = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    float *headPivot = (float *)(Message->args + index);
    index += 3*sizeof(float *);
    float *aabb = (float *)(Message->args + index);
    index += 6*sizeof(float *);

    getAnimalGeometry(geometrySet, hash, positions, colors, indices, heads, legs, numPositions, numColors, numIndices, numHeads, numLegs, headPivot, aabb);

    threadPool->outbox.push(Message);

    /* unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *nameData = *((char **)(Message->args + index));
    index += sizeof(char *);
    unsigned int nameSize = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    float **positions = (float **)(Message->args + index);
    index += sizeof(float **);
    float **uvs = (float **)(Message->args + index);
    index += sizeof(float **);
    unsigned int **indices = (unsigned int **)(Message->args + index);
    index += sizeof(unsigned int **);
    unsigned int *numPositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numUvs = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numIndices = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);

    getGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, numPositions, numUvs, numIndices); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // marchObjects
    abort();
    /* unsigned int index = 0;
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    int x = *((int *)(Message->args + index));
    index += sizeof(int);
    int y = *((int *)(Message->args + index));
    index += sizeof(int);
    int z = *((int *)(Message->args + index));
    index += sizeof(int);

    // std::cout << "march objects a " << x << " " << y << " " << z << std::endl;

    Subparcel *subparcel = *((Subparcel **)(Message->args + index));
    index += sizeof(Subparcel *);
    Subparcel *subparcels = *((Subparcel **)(Message->args + index));
    index += sizeof(Subparcel *);
    unsigned int numSubparcels = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    ArenaAllocator *positionsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *uvsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *idsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *indicesAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *skyLightsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *torchLightsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    FreeEntry **positionsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **uvsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **idsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **indicesEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **skyLightsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **torchLightsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);

    // std::cout << "march objects ax " << (unsigned int)(void *)geometrySet << " " << (unsigned int)(void *)marchObjects << " " << numMarchObjects << std::endl;

    unsigned int numPositions;
    unsigned int numUvs;
    unsigned int numIds;
    unsigned int numIndices;
    unsigned int numSkyLights;
    unsigned int numTorchLights;
    doGetMarchObjectStats(geometrySet, subparcel, numPositions, numUvs, numIds, numIndices, numSkyLights, numTorchLights);

    // std::cout << "march objects b " << numPositions << " " << numUvs << " " << numIds << " " << numIndices << " " << numSkyLights << " " << numTorchLights << " " << (unsigned int)(void *)positionsAllocator << std::endl;

    *positionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
    if (!*positionsEntry) {
      std::cout << "could not allocate marchObjects positions" << std::endl;
      abort();
    }
    *uvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
    if (!*uvsEntry) {
      std::cout << "could not allocate marchObjects uvs" << std::endl;
      abort();
    }
    *idsEntry = idsAllocator->alloc(numIds*sizeof(float));
    if (!*idsEntry) {
      std::cout << "could not allocate marchObjects ids" << std::endl;
      abort();
    }
    *indicesEntry = indicesAllocator->alloc(numIndices*sizeof(unsigned int));
    if (!*indicesEntry) {
      std::cout << "could not allocate marchObjects indices" << std::endl;
      abort();
    }
    *skyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
    if (!*skyLightsEntry) {
      std::cout << "could not allocate marchObjects skyLights" << std::endl;
      abort();
    }
    *torchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
    if (!*torchLightsEntry) {
      std::cout << "could not allocate marchObjects torchLights" << std::endl;
      abort();
    }

    // std::cout << "march objects c " << numPositions << " " << numUvs << " " << numIds << " " << numIndices << " " << numSkyLights << " " << numTorchLights << std::endl;

    float *positions = (float *)(positionsAllocator->data + (*positionsEntry)->start);
    float *uvs = (float *)(uvsAllocator->data + (*uvsEntry)->start);
    float *ids = (float *)(idsAllocator->data + (*idsEntry)->start);
    unsigned int *indices = (unsigned int *)(indicesAllocator->data + (*indicesEntry)->start);
    unsigned char *skyLights = (unsigned char *)(skyLightsAllocator->data + (*skyLightsEntry)->start);
    unsigned char *torchLights = (unsigned char *)(torchLightsAllocator->data + (*torchLightsEntry)->start);

    unsigned int indexOffset = (*positionsEntry)->start/sizeof(float)/3;

    doMarchObjects(geometrySet, x, y, z, subparcel, subparcels, numSubparcels, positions, uvs, ids, indices, skyLights, torchLights, indexOffset);

    // std::cout << "march objects d " << numIndices << " " << indices[0] << " " << indices[1] << " " << indices[2] << " " << (unsigned int)(void *)positionsAllocator << " " << (unsigned int)(void *)indicesAllocator << " " << (unsigned int)(void *)indicesAllocator->data << " " << (*indicesEntry)->start << std::endl; */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // getHeight
    unsigned int index = 0;
    int seed = *((int *)(Message->args + index));
    index += sizeof(int);
    float x = *((float *)(Message->args + index));
    index += sizeof(float);
    float y = *((float *)(Message->args + index));
    index += sizeof(float);
    float z = *((float *)(Message->args + index));
    index += sizeof(float);
    float baseHeight = *((float *)(Message->args + index));
    index += sizeof(float);
    float *height = (float *)(Message->args + index);
    index += sizeof(float *);
    *height = getHeight(seed, x, y, z, baseHeight);

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // noise
    abort();
    /* unsigned int index = 0;

    int seed = *((int *)(Message->args + index));
    index += sizeof(int);

    float x = *((float *)(Message->args + index));
    index += sizeof(float);
    float y = *((float *)(Message->args + index));
    index += sizeof(float);
    float z = *((float *)(Message->args + index));
    index += sizeof(float);

    float baseHeight = *((float *)(Message->args + index));
    index += sizeof(float);
    float wormRate = *((float *)(Message->args + index));
    index += sizeof(float);
    float wormRadiusBase = *((float *)(Message->args + index));
    index += sizeof(float);
    float wormRadiusRate = *((float *)(Message->args + index));
    index += sizeof(float);
    float objectsRate = *((float *)(Message->args + index));
    index += sizeof(float);
    float potentialDefault = *((float *)(Message->args + index));
    index += sizeof(float);

    void *subparcelByteOffset = *((void **)(Message->args + index));
    index += sizeof(void *);

    noise3(seed, x, y, z, baseHeight, wormRate, wormRadiusBase, wormRadiusRate, objectsRate, potentialDefault, subparcelByteOffset); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // marchingCubes
    abort();
    /* unsigned int index = 0;

    float meshId = *(float *)(Message->args + index);
    index += sizeof(float);

    int *dims = (int *)(Message->args + index);
    index += 3*sizeof(int);

    float *potential = *((float **)(Message->args + index));
    index += sizeof(float *);
    unsigned char *biomes = *((unsigned char **)(Message->args + index));
    index += sizeof(unsigned char *);
    char *heightfield = *((char **)(Message->args + index));
    index += sizeof(char *);
    unsigned char *lightfield = *((unsigned char **)(Message->args + index));
    index += sizeof(unsigned char *);

    float *shift = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float *scale = (float *)(Message->args + index);
    index += 3*sizeof(float);

    ArenaAllocator *positionsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *normalsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *uvsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    // ArenaAllocator *barycentricsAllocator = *((ArenaAllocator **)(Message->args + index));
    // index += sizeof(ArenaAllocator *);
    ArenaAllocator *aosAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *idsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *skyLightsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *torchLightsAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);
    ArenaAllocator *peeksAllocator = *((ArenaAllocator **)(Message->args + index));
    index += sizeof(ArenaAllocator *);

    FreeEntry **positionsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **normalsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **uvsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    // FreeEntry **barycentricsEntry = (FreeEntry **)(Message->args + index);
    // index += sizeof(FreeEntry *);
    FreeEntry **aosEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **idsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **skyLightsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **torchLightsEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);
    FreeEntry **peeksEntry = (FreeEntry **)(Message->args + index);
    index += sizeof(FreeEntry *);

    unsigned int *numOpaquePositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);
    unsigned int *numTransparentPositions = (unsigned int *)(Message->args + index);
    index += sizeof(unsigned int *);

    constexpr int bufferSize = 1024*1024;
    std::vector<float> positions(bufferSize);
    std::vector<float> normals(bufferSize);
    std::vector<float> uvs(bufferSize);
    // std::vector<float> barycentrics(bufferSize);
    std::vector<unsigned char> aos(bufferSize);
    std::vector<float> ids(bufferSize);
    std::vector<unsigned char> skyLights(bufferSize);
    std::vector<unsigned char> torchLights(bufferSize);
    constexpr unsigned int numPeeks = 15;
    std::vector<unsigned char> peeks(numPeeks);
    unsigned int numPositions;
    unsigned int numNormals;
    unsigned int numUvs;
    // unsigned int numBarycentrics;
    unsigned int numAos;
    unsigned int numIds;
    unsigned int numSkyLights;
    unsigned int numTorchLights;
    marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions.data(), normals.data(), uvs.data(), aos.data(), ids.data(), skyLights.data(), torchLights.data(), numPositions, numNormals, numUvs, numAos, numIds, numSkyLights, numTorchLights, *numOpaquePositions, *numTransparentPositions, peeks.data());

    *positionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
    if (!*positionsEntry) {
      std::cout << "could not allocate marchingCubes positions" << std::endl;
      abort();
    }
    *normalsEntry = normalsAllocator->alloc(numNormals*sizeof(float));
    if (!*normalsEntry) {
      std::cout << "could not allocate marchingCubes normals" << std::endl;
      abort();
    }
    *uvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
    if (!*uvsEntry) {
      std::cout << "could not allocate marchingCubes uvs" << std::endl;
      abort();
    }
    // *barycentricsEntry = barycentricsAllocator->alloc(numBarycentrics*sizeof(float));
    // if (!*barycentricsEntry) {
      // std::cout << "could not allocate marchingCubes barycentrics" << std::endl;
      // abort();
    // }
    *aosEntry = aosAllocator->alloc(numAos*sizeof(float));
    if (!*aosEntry) {
      std::cout << "could not allocate marchingCubes aos" << std::endl;
      abort();
    }
    *idsEntry = idsAllocator->alloc(numIds*sizeof(float));
    if (!*idsEntry) {
      std::cout << "could not allocate marchingCubes ids" << std::endl;
      abort();
    }
    *skyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
    if (!*skyLightsEntry) {
      std::cout << "could not allocate marchingCubes skyLights" << std::endl;
      abort();
    }
    *torchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
    if (!*torchLightsEntry) {
      std::cout << "could not allocate marchingCubes torchLights" << std::endl;
      abort();
    }
    *peeksEntry = peeksAllocator->alloc(numPeeks*sizeof(unsigned char));
    if (!*peeksEntry) {
      std::cout << "could not allocate marchingCubes peeks" << std::endl;
      abort();
    }

    memcpy(positionsAllocator->data + (*positionsEntry)->start, positions.data(), numPositions*sizeof(float));
    memcpy(normalsAllocator->data + (*normalsEntry)->start, normals.data(), numNormals*sizeof(float));
    memcpy(uvsAllocator->data + (*uvsEntry)->start, uvs.data(), numUvs*sizeof(float));
    // memcpy(barycentricsAllocator->data + (*barycentricsEntry)->start, barycentrics.data(), numBarycentrics*sizeof(float));
    memcpy(aosAllocator->data + (*aosEntry)->start, aos.data(), numAos*sizeof(unsigned char));
    memcpy(idsAllocator->data + (*idsEntry)->start, ids.data(), numIds*sizeof(float));
    memcpy(skyLightsAllocator->data + (*skyLightsEntry)->start, skyLights.data(), numSkyLights*sizeof(unsigned char));
    memcpy(torchLightsAllocator->data + (*torchLightsEntry)->start, torchLights.data(), numTorchLights*sizeof(unsigned char));
    memcpy(peeksAllocator->data + (*peeksEntry)->start, peeks.data(), numPeeks*sizeof(unsigned char)); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // bakeGeometry
    abort();
    /* unsigned int index = 0;

    float *positions = *((float **)(Message->args + index));
    index += sizeof(float *);

    unsigned int *indices = *((unsigned int **)(Message->args + index));
    index += sizeof(unsigned int *);

    unsigned int numPositions = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    unsigned int numIndices = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    PxDefaultMemoryOutputStream **writeStream = (PxDefaultMemoryOutputStream **)(Message->args + index);
    index += sizeof(PxDefaultMemoryOutputStream *);

    // std::cout << "bake 1" << std::endl;
    bakeGeometry(positions, indices, numPositions, numIndices, writeStream);
    // std::cout << "bake 2" << std::endl; */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // getSubparcel
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    int x = *((int *)(Message->args + index));
    index += sizeof(int);
    int y = *((int *)(Message->args + index));
    index += sizeof(void *);
    int z = *((int *)(Message->args + index));
    index += sizeof(void *);

    std::shared_ptr<Subparcel> subparcel;
    {
      int subparcelIndex = getSubparcelIndex(x, y, z);
      std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
      auto subparcelIter = tracker->subparcels.find(subparcelIndex);
      if (subparcelIter != tracker->subparcels.end()) {
        subparcel = subparcelIter->second;
      }
    }
    std::shared_ptr<Subparcel> *subparcelPtr;
    if (subparcel) {
      subparcelPtr = new std::shared_ptr<Subparcel>(subparcel);
    } else {
      subparcelPtr = nullptr;
    }

    index = 0;
    *((std::shared_ptr<Subparcel> **)(Message->args + index)) = subparcelPtr;
    index += sizeof(std::shared_ptr<Subparcel> *);
    *((Subparcel **)(Message->args + index)) = subparcel.get();
    index += sizeof(Subparcel *);

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // releaseSubparcel
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    std::shared_ptr<Subparcel> *subparcelSharedPtr = *((std::shared_ptr<Subparcel> **)(Message->args + index));
    index += sizeof(std::shared_ptr<Subparcel> *);

    {
      std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
      if ((*subparcelSharedPtr)->live) {
        tracker->subparcels[(*subparcelSharedPtr)->coord.index] = *subparcelSharedPtr;
        tracker->loadingSubparcels.erase((*subparcelSharedPtr)->coord.index);
      }
    }
    delete subparcelSharedPtr;

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // chunk
    unsigned int index = 0;
    int seed = *((int *)(Message->args + index));
    index += sizeof(int);
    void *trackerByteOffset = *((void **)(Message->args + index));
    index += sizeof(void *);
    void *geometrySetByteOffset = *((void **)(Message->args + index));
    index += sizeof(void *);
    void *subparcelSharedPtrByteOffset = *((void **)(Message->args + index));
    index += sizeof(void *);

    Tracker *tracker = (Tracker *)trackerByteOffset;
    GeometrySet *geometrySet = (GeometrySet *)geometrySetByteOffset;
    std::shared_ptr<Subparcel> *subparcelSharedPtr = (std::shared_ptr<Subparcel> *)subparcelSharedPtrByteOffset;

    if ((*subparcelSharedPtr)->live) { // check for abort
      std::shared_ptr<Subparcel> subparcel(*subparcelSharedPtr);

      {
        constexpr float baseHeight = (float)PARCEL_SIZE/2.0f-10.0f;
        constexpr float wormRate = 2;
        constexpr float wormRadiusBase = 2;
        constexpr float wormRadiusRate = 2;
        constexpr float objectsRate = 3;
        constexpr float potentialDefault = -0.5;

        // std::cout << "execute chunk " << subparcel->coord.x << " " << subparcel->coord.y << " " << subparcel->coord.z << std::endl;
        noise3(seed, subparcel->coord.x, subparcel->coord.y, subparcel->coord.z, baseHeight, wormRate, wormRadiusBase, wormRadiusRate, objectsRate, potentialDefault, subparcel.get());
      }

      float *landPositions;
      unsigned int numLandPositions;
      {
        float meshId = tracker->meshId;
        int dims[3] = {
          SUBPARCEL_SIZE,
          SUBPARCEL_SIZE,
          SUBPARCEL_SIZE,
        };
        float *potential = subparcel->potentials;
        unsigned char *biomes = subparcel->biomes;
        char *heightfield = subparcel->heightfield;
        unsigned char *lightfield = subparcel->lightfield;
        float shift[3] = {
          (float)subparcel->coord.x*(float)SUBPARCEL_SIZE,
          (float)subparcel->coord.y*(float)SUBPARCEL_SIZE,
          (float)subparcel->coord.z*(float)SUBPARCEL_SIZE,
        };
        float scale[3] = {1, 1, 1};

        ArenaAllocator *positionsAllocator = tracker->landPositionsAllocator;
        ArenaAllocator *normalsAllocator = tracker->landNormalsAllocator;
        ArenaAllocator *uvsAllocator = tracker->landUvsAllocator;
        // ArenaAllocator *barycentricsAllocator = tracker->landBarycentricsAllocator;
        ArenaAllocator *aosAllocator = tracker->landAosAllocator;
        ArenaAllocator *idsAllocator = tracker->landIdsAllocator;
        ArenaAllocator *skyLightsAllocator = tracker->landSkyLightsAllocator;
        ArenaAllocator *torchLightsAllocator = tracker->landTorchLightsAllocator;

        unsigned int numOpaquePositions;
        unsigned int numTransparentPositions;

        constexpr int bufferSize = 1024*1024;
        std::vector<float> positions(bufferSize);
        std::vector<float> normals(bufferSize);
        std::vector<float> uvs(bufferSize);
        // std::vector<float> barycentrics(bufferSize);
        std::vector<unsigned char> aos(bufferSize);
        std::vector<float> ids(bufferSize);
        std::vector<unsigned char> skyLights(bufferSize);
        std::vector<unsigned char> torchLights(bufferSize);
        constexpr unsigned int numPeeks = 15;
        std::vector<unsigned char> peeks(numPeeks);
        unsigned int numPositions;
        unsigned int numNormals;
        unsigned int numUvs;
        // unsigned int numBarycentrics;
        unsigned int numAos;
        unsigned int numIds;
        unsigned int numSkyLights;
        unsigned int numTorchLights;
        marchingCubes2(meshId, dims, potential, biomes, heightfield, lightfield, shift, scale, positions.data(), normals.data(), uvs.data(), /*barycentrics.data(),*/ aos.data(), ids.data(), skyLights.data(), torchLights.data(), numPositions, numNormals, numUvs, /*numBarycentrics,*/ numAos, numIds, numSkyLights, numTorchLights, numOpaquePositions, numTransparentPositions, peeks.data());

        subparcel->landPositionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
        if (!subparcel->landPositionsEntry) {
          std::cout << "could not allocate chunk marchingCubes positions" << std::endl;
          abort();
        }
        subparcel->landNormalsEntry = normalsAllocator->alloc(numNormals*sizeof(float));
        if (!subparcel->landNormalsEntry) {
          std::cout << "could not allocate chunk marchingCubes normals" << std::endl;
          abort();
        }
        subparcel->landUvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
        if (!subparcel->landUvsEntry) {
          std::cout << "could not allocate chunk marchingCubes uvs" << std::endl;
          abort();
        }
        /* subparcel->landBarycentricsEntry = barycentricsAllocator->alloc(numBarycentrics*sizeof(float)); // XXX maybe not needed
        if (!subparcel->landBarycentricsEntry) {
          std::cout << "could not allocate chunk marchingCubes barycentrics" << std::endl;
          abort();
        } */
        subparcel->landAosEntry = aosAllocator->alloc(numAos*sizeof(unsigned char));
        if (!subparcel->landAosEntry) {
          std::cout << "could not allocate chunk marchingCubes aos" << std::endl;
          abort();
        }
        subparcel->landIdsEntry = idsAllocator->alloc(numIds*sizeof(float));
        if (!subparcel->landIdsEntry) {
          std::cout << "could not allocate chunk marchingCubes ids" << std::endl;
          abort();
        }
        subparcel->landSkyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
        if (!subparcel->landSkyLightsEntry) {
          std::cout << "could not allocate chunk marchingCubes skyLights" << std::endl;
          abort();
        }
        subparcel->landTorchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
        if (!subparcel->landTorchLightsEntry) {
          std::cout << "could not allocate chunk marchingCubes torchLights" << std::endl;
          abort();
        }

        memcpy(positionsAllocator->data + subparcel->landPositionsEntry->spec.start, positions.data(), numPositions*sizeof(float));
        memcpy(normalsAllocator->data + subparcel->landNormalsEntry->spec.start, normals.data(), numNormals*sizeof(float));
        memcpy(uvsAllocator->data + subparcel->landUvsEntry->spec.start, uvs.data(), numUvs*sizeof(float));
        // memcpy(barycentricsAllocator->data + subparcel->landBarycentricsEntry->start, barycentrics.data(), numBarycentrics*sizeof(float));
        memcpy(aosAllocator->data + subparcel->landAosEntry->spec.start, aos.data(), numAos*sizeof(unsigned char));
        memcpy(idsAllocator->data + subparcel->landIdsEntry->spec.start, ids.data(), numIds*sizeof(float));
        memcpy(skyLightsAllocator->data + subparcel->landSkyLightsEntry->spec.start, skyLights.data(), numSkyLights*sizeof(unsigned char));
        memcpy(torchLightsAllocator->data + subparcel->landTorchLightsEntry->spec.start, torchLights.data(), numTorchLights*sizeof(unsigned char));
        memcpy(subparcel->peeks, peeks.data(), numPeeks*sizeof(unsigned char));

        // groups
        subparcel->landGroups[0].start = subparcel->landPositionsEntry->spec.start/sizeof(float)/3;
        subparcel->landGroups[0].count = numOpaquePositions/3;
        subparcel->landGroups[0].materialIndex = 0;
        subparcel->landGroups[1].start = subparcel->landGroups[0].start + subparcel->landGroups[0].count;
        subparcel->landGroups[1].count = numTransparentPositions/3;
        subparcel->landGroups[1].materialIndex = 1;

        // latch
        landPositions = (float *)(positionsAllocator->data + subparcel->landPositionsEntry->spec.start);
        numLandPositions = numOpaquePositions;
      }
      {
        // std::cout << "march objects ax " << (unsigned int)(void *)geometrySet << " " << (unsigned int)(void *)marchObjects << " " << numMarchObjects << std::endl;

        unsigned int numPositions;
        unsigned int numUvs;
        unsigned int numAtlasUvs;
        unsigned int numIds;
        unsigned int numIndices;
        unsigned int numSkyLights;
        unsigned int numTorchLights;
        doGetMarchObjectStats(geometrySet, subparcel.get(), numPositions, numUvs, numAtlasUvs, numIds, numIndices, numSkyLights, numTorchLights);

        ArenaAllocator *positionsAllocator = tracker->vegetationPositionsAllocator;
        ArenaAllocator *uvsAllocator = tracker->vegetationUvsAllocator;
        ArenaAllocator *atlasUvsAllocator = tracker->vegetationAtlasUvsAllocator;
        ArenaAllocator *idsAllocator = tracker->vegetationIdsAllocator;
        ArenaAllocator *indicesAllocator = tracker->vegetationIndicesAllocator;
        ArenaAllocator *skyLightsAllocator = tracker->vegetationSkyLightsAllocator;
        ArenaAllocator *torchLightsAllocator = tracker->vegetationTorchLightsAllocator;

        subparcel->vegetationPositionsEntry = positionsAllocator->alloc(numPositions*sizeof(float));
        if (!subparcel->vegetationPositionsEntry) {
          std::cout << "could not allocate chunk marchObjects positions" << std::endl;
          abort();
        }
        subparcel->vegetationUvsEntry = uvsAllocator->alloc(numUvs*sizeof(float));
        if (!subparcel->vegetationUvsEntry) {
          std::cout << "could not allocate chunk marchObjects uvs" << std::endl;
          abort();
        }
        subparcel->vegetationAtlasUvsEntry = atlasUvsAllocator->alloc(numAtlasUvs*sizeof(float));
        if (!subparcel->vegetationAtlasUvsEntry) {
          std::cout << "could not allocate chunk marchObjects uvs" << std::endl;
          abort();
        }
        subparcel->vegetationIdsEntry = idsAllocator->alloc(numIds*sizeof(float));
        if (!subparcel->vegetationIdsEntry) {
          std::cout << "could not allocate chunk marchObjects ids" << std::endl;
          abort();
        }
        subparcel->vegetationIndicesEntry = indicesAllocator->alloc(numIndices*sizeof(unsigned int));
        if (!subparcel->vegetationIndicesEntry) {
          std::cout << "could not allocate chunk marchObjects indices" << std::endl;
          abort();
        }
        subparcel->vegetationSkyLightsEntry = skyLightsAllocator->alloc(numSkyLights*sizeof(unsigned char));
        if (!subparcel->vegetationSkyLightsEntry) {
          std::cout << "could not allocate chunk marchObjects skyLights" << std::endl;
          abort();
        }
        subparcel->vegetationTorchLightsEntry = torchLightsAllocator->alloc(numTorchLights*sizeof(unsigned char));
        if (!subparcel->vegetationTorchLightsEntry) {
          std::cout << "could not allocate chunk marchObjects torchLights" << std::endl;
          abort();
        }

        float *positions = (float *)(positionsAllocator->data + subparcel->vegetationPositionsEntry->spec.start);
        float *uvs = (float *)(uvsAllocator->data + subparcel->vegetationUvsEntry->spec.start);
        float *atlasUvs = (float *)(atlasUvsAllocator->data + subparcel->vegetationAtlasUvsEntry->spec.start);
        float *ids = (float *)(idsAllocator->data + subparcel->vegetationIdsEntry->spec.start);
        unsigned int *indices = (unsigned int *)(indicesAllocator->data + subparcel->vegetationIndicesEntry->spec.start);
        unsigned char *skyLights = (unsigned char *)(skyLightsAllocator->data + subparcel->vegetationSkyLightsEntry->spec.start);
        unsigned char *torchLights = (unsigned char *)(torchLightsAllocator->data + subparcel->vegetationTorchLightsEntry->spec.start);

        /* slab => (slab.position.byteOffset - geometry.attributes.position.array.byteOffset)/Float32Array.BYTES_PER_ELEMENT;
        const indexOffset = vegetationMesh.getSlabPositionOffset(slab)/3;
        for (let i = 0; i < spec.indices.length; i++) {
          spec.indices[i] += indexOffset;
        } */
        Subparcel *subparcels = nullptr;
        unsigned int numSubparcels = 0;
        unsigned int indexOffset = subparcel->vegetationPositionsEntry->spec.start/sizeof(float)/3;
        doMarchObjects(tracker, geometrySet, subparcel->coord.x, subparcel->coord.y, subparcel->coord.z, subparcel.get(), subparcels, numSubparcels, positions, uvs, atlasUvs, ids, indices, skyLights, torchLights, indexOffset);

        // groups
        subparcel->vegetationGroups[0].start = subparcel->vegetationIndicesEntry->spec.start/sizeof(unsigned int);
        subparcel->vegetationGroups[0].count = subparcel->vegetationIndicesEntry->spec.count/sizeof(unsigned int);
        subparcel->vegetationGroups[0].materialIndex = 0;
      }
      doLandPhysics(tracker, subparcel.get(), landPositions, numLandPositions);
      doObjectPhysics(tracker, geometrySet, subparcel.get());

      // output
      // std::cout << "return 1" << std::endl;
      Message->method = (int)MESSAGES::updateGeometry;

      // std::cout << "return 2" << std::endl;

      index = 0;
      *((FreeEntry **)(Message->args + index)) = subparcel->landPositionsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landNormalsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landUvsEntry.get();
      index += sizeof(FreeEntry *);
      /* *((FreeEntry **)(Message->args + index)) = subparcel->landBarycentricsEntry.get();
      index += sizeof(FreeEntry *); */
      *((FreeEntry **)(Message->args + index)) = subparcel->landAosEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landIdsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landSkyLightsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->landTorchLightsEntry.get();
      index += sizeof(FreeEntry *);

      // std::cout << "return 3" << std::endl;

      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationPositionsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationUvsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationAtlasUvsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIdsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIndicesEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationSkyLightsEntry.get();
      index += sizeof(FreeEntry *);
      *((FreeEntry **)(Message->args + index)) = subparcel->vegetationTorchLightsEntry.get();
      index += sizeof(FreeEntry *);

      *((std::shared_ptr<Subparcel> **)(Message->args + index)) = subparcelSharedPtr;
      index += sizeof(std::shared_ptr<Subparcel> *);

      // std::cout << "return 4" << std::endl;

      if (index/sizeof(unsigned int) > Message->count) {
        std::cout << "message overflow " << index << " " << Message->count << std::endl;
        abort();
      }

      threadPool->outbox.push(Message);

      // std::cout << "return update " << (void *)subparcelSharedPtr << " " << subparcel->coord.x << " " << subparcel->coord.y << " " << subparcel->coord.z << std::endl;
    }
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // mine
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float delta = *((float *)(Message->args + index));
    index += sizeof(float);

    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doMine(tracker, position, delta);
    const bool &doable = spec.first;
    if (doable) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->landPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landNormalsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landAosEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      std::vector<int> indices;
      {
        int x = (int)std::floor(position[0]);
        int y = (int)std::floor(position[1]);
        int z = (int)std::floor(position[2]);
        constexpr int radius = 1;
        for (int dy = -radius - 1; dy <= radius + 1; dy++) {
          const int ay = y + dy;
          for (int dz = -radius - 1; dz <= radius + 1; dz++) {
            const int az = z + dz;
            for (int dx = -radius - 1; dx <= radius + 1; dx++) {
              const int ax = x + dx;
              const int sx = (int)std::floor((float)ax/(float)SUBPARCEL_SIZE);
              const int sy = (int)std::floor((float)ay/(float)SUBPARCEL_SIZE);
              const int sz = (int)std::floor((float)az/(float)SUBPARCEL_SIZE);
              const int index = getSubparcelIndex(sx, sy, sz);
              auto iter = std::find(indices.begin(), indices.end(), index);
              if (iter == indices.end()) {
                indices.push_back(index);
              }
            }
          }
        }
      }
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // releaseMine
    unsigned int index = 0;

    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);

    unsigned int numSubparcels = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    {
      std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
      for (unsigned int i = 0; i < numSubparcels; i++) {
        std::shared_ptr<Subparcel> *subparcelSharedPtr = *((std::shared_ptr<Subparcel> **)(Message->args + index));
        index += sizeof(std::shared_ptr<Subparcel> *);

        if ((*subparcelSharedPtr)->live) {
          tracker->subparcels[(*subparcelSharedPtr)->coord.index] = *subparcelSharedPtr;
        }
        delete subparcelSharedPtr;
      }
    }

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // light
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float delta = *((float *)(Message->args + index));
    index += sizeof(float);

    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doLight(tracker, position, delta);
    const bool &doable = spec.first;
    if (doable) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->landPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landNormalsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landAosEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->landTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      std::vector<int> indices;
      {
        int x = (int)std::floor(position[0]);
        int y = (int)std::floor(position[1]);
        int z = (int)std::floor(position[2]);
        constexpr int radius = 1;
        for (int dy = -radius - 1; dy <= radius + 1; dy++) {
          const int ay = y + dy;
          for (int dz = -radius - 1; dz <= radius + 1; dz++) {
            const int az = z + dz;
            for (int dx = -radius - 1; dx <= radius + 1; dx++) {
              const int ax = x + dx;
              const int sx = (int)std::floor((float)ax/(float)SUBPARCEL_SIZE);
              const int sy = (int)std::floor((float)ay/(float)SUBPARCEL_SIZE);
              const int sz = (int)std::floor((float)az/(float)SUBPARCEL_SIZE);
              const int index = getSubparcelIndex(sx, sy, sz);
              auto iter = std::find(indices.begin(), indices.end(), index);
              if (iter == indices.end()) {
                indices.push_back(index);
              }
            }
          }
        }
      }
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }

    /* unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float delta = *((float *)(Message->args + index));
    index += sizeof(float);

    std::vector<std::shared_ptr<Subparcel>> mineSpecs = doLight(tracker, position, delta); */
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // addObject
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *name = (char *)(Message->args + index);
    index += MAX_NAME_LENGTH;
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float *quaternion = (float *)(Message->args + index);
    index += 4*sizeof(float);

    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doAddObject(tracker, geometrySet, OBJECT_TYPE::VEGETATION, name, position, quaternion);
    if (spec.first) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      for (const std::shared_ptr<Subparcel> &subparcel : newSubparcels) {
        doObjectPhysics(tracker, geometrySet, subparcel.get());
      }

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationAtlasUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIndicesEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      const int sx = (int)std::floor(position[0]/(float)SUBPARCEL_SIZE);
      const int sy = (int)std::floor(position[1]/(float)SUBPARCEL_SIZE);
      const int sz = (int)std::floor(position[2]/(float)SUBPARCEL_SIZE);
      const int subparcelIndex = getSubparcelIndex(sx, sy, sz);
      std::vector<int> indices{subparcelIndex};
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // removeObject
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    int sx = *((int *)(Message->args + index));
    index += sizeof(int);
    int sy = *((int *)(Message->args + index));
    index += sizeof(int);
    int sz = *((int *)(Message->args + index));
    index += sizeof(int);
    unsigned int objectId = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    const int subparcelIndex = getSubparcelIndex(sx, sy, sz);
    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doRemoveObject(tracker, geometrySet, subparcelIndex, objectId);
    if (spec.first) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      for (const std::shared_ptr<Subparcel> &subparcel : newSubparcels) {
        doObjectPhysics(tracker, geometrySet, subparcel.get());
      }

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationAtlasUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIndicesEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      std::vector<int> indices{subparcelIndex};
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // releaseAddRemoveObject
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    unsigned int numSubparcels = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    {
      std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
      for (unsigned int i = 0; i < numSubparcels; i++) {
        std::shared_ptr<Subparcel> *subparcelSharedPtr = *((std::shared_ptr<Subparcel> **)(Message->args + index));
        index += sizeof(std::shared_ptr<Subparcel> *);

        if ((*subparcelSharedPtr)->live) {
          tracker->subparcels[(*subparcelSharedPtr)->coord.index] = *subparcelSharedPtr;
        }
        delete subparcelSharedPtr;
      }
    }

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // addThingGeometry
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *nameCharPtr = (char *)(Message->args + index);
    index += MAX_NAME_LENGTH;
    float *positions = *((float **)(Message->args + index));
    index += sizeof(float *);
    float *uvs = *((float **)(Message->args + index));
    index += sizeof(float *);
    unsigned int *indices = *((unsigned int **)(Message->args + index));
    index += sizeof(unsigned int *);
    unsigned int numPositions = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    unsigned int numUvs = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    unsigned int numIndices = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);
    unsigned char *texture = *((unsigned char **)(Message->args + index));
    index += sizeof(unsigned char *);
    unsigned int textureLength = *((unsigned int *)(Message->args + index));
    index += sizeof(unsigned int);

    Geometry *geometry = new Geometry();
    geometry->name = std::string(nameCharPtr);
    geometry->positions.resize(numPositions);
    memcpy(geometry->positions.data(), positions, numPositions * sizeof(float));
    geometry->uvs.resize(numUvs);
    memcpy(geometry->uvs.data(), uvs, numUvs * sizeof(float));
    geometry->indices.resize(numIndices);
    memcpy(geometry->indices.data(), indices, numIndices * sizeof(float));
    geometry->aabb.setFromPositions(positions, numPositions);
    geometry->texture = texture;

    PxDefaultMemoryOutputStream *writeStream = doBakeGeometry(&tracker->physicer, positions, indices, numPositions, numIndices);
    std::pair<PxTriangleMesh *, PxTriangleMeshGeometry *> spec = doMakeBakedGeometryRaw(&tracker->physicer, writeStream); // XXX GC the TriangleMesh
    geometry->physxGeometry = spec.second;

    geometrySet->thingGeometries.push_back(geometry);
    geometrySet->geometryMap[geometry->name] = geometry;

    threadPool->outbox.push(Message);
  },
  [](ThreadPool *threadPool, Message *Message) -> void { // addThing
    unsigned int index = 0;
    Tracker *tracker = *((Tracker **)(Message->args + index));
    index += sizeof(Tracker *);
    GeometrySet *geometrySet = *((GeometrySet **)(Message->args + index));
    index += sizeof(GeometrySet *);
    char *name = (char *)(Message->args + index);
    index += MAX_NAME_LENGTH;
    float *position = (float *)(Message->args + index);
    index += 3*sizeof(float);
    float *quaternion = (float *)(Message->args + index);
    index += 4*sizeof(float *);

    std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> spec = doAddThing(tracker, geometrySet, name, position, quaternion);
    if (spec.first) {
      std::vector<std::shared_ptr<Subparcel>> &newSubparcels = spec.second;

      for (const std::shared_ptr<Subparcel> &subparcel : newSubparcels) {
        doObjectPhysics(tracker, geometrySet, subparcel.get());
      }

      index = 0;
      *((unsigned int *)(Message->args + index)) = newSubparcels.size();
      index += sizeof(unsigned int);
      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationPositionsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationAtlasUvsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIdsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationIndicesEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationSkyLightsEntry.get();
        index += sizeof(FreeEntry *);
        *((FreeEntry **)(Message->args + index)) = subparcel->vegetationTorchLightsEntry.get();
        index += sizeof(FreeEntry *);
      }

      for (unsigned int i = 0; i < newSubparcels.size(); i++) {
        std::shared_ptr<Subparcel> &subparcel = newSubparcels[i];
        *((std::shared_ptr<Subparcel> **)(Message->args + index)) = new std::shared_ptr<Subparcel>(subparcel);
        index += sizeof(std::shared_ptr<Subparcel> *);
      }

      threadPool->outbox.push(Message);
    } else {
      const int sx = (int)std::floor(position[0]/(float)SUBPARCEL_SIZE);
      const int sy = (int)std::floor(position[1]/(float)SUBPARCEL_SIZE);
      const int sz = (int)std::floor(position[2]/(float)SUBPARCEL_SIZE);
      const int subparcelIndex = getSubparcelIndex(sx, sy, sz);
      std::vector<int> indices{subparcelIndex};
      std::function<bool()> guardFn = [tracker, indices{std::move(indices)}]() -> bool {
        return checkSubparcelIndicesLive(tracker, indices);
      };
      threadPool->dependencyInbox.push(guardFn, Message);
    }
  },
};

}