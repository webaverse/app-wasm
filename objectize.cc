#include <emscripten.h>
#include "geometry.h"
// #include <iostream>

extern "C" {

EMSCRIPTEN_KEEPALIVE void *makeGeometrySet() {
  return doMakeGeometrySet();
}

EMSCRIPTEN_KEEPALIVE void loadBake(GeometrySet *geometrySet, unsigned char *data, unsigned int size) {
  doLoadBake(geometrySet, data, size);
}

EMSCRIPTEN_KEEPALIVE void getGeometry(GeometrySet *geometrySet, char *nameData, unsigned int nameSize, float **positions, float **uvs, unsigned int **indices, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIndices) {
  doGetGeometry(geometrySet, nameData, nameSize, positions, uvs, indices, *numPositions, *numUvs, *numIndices);
}

EMSCRIPTEN_KEEPALIVE void getAnimalGeometry(GeometrySet *geometrySet, unsigned int hash, float **positions, float **colors, float **heads, float **legs, unsigned int **indices, unsigned int *numPositions, unsigned int *numColors, unsigned int *numIndices, unsigned int *numHeads, unsigned int *numLegs, float *headPivot, float *aabb) {
  doGetAnimalGeometry(geometrySet, hash, positions, colors, heads, legs, indices, *numPositions, *numColors, *numIndices, *numHeads, *numLegs, headPivot, aabb);
}

EMSCRIPTEN_KEEPALIVE void marchObjects(GeometrySet *geometrySet, int x, int y, int z, MarchObject *marchObjects, unsigned int numMarchObjects, MarchSpec *marchSpecs, unsigned int numMarchSpecs, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIds, unsigned int *numIndices) {
  doMarchObjects(geometrySet, x, y, z, marchObjects, numMarchObjects, marchSpecs, numMarchSpecs, positions, uvs, ids, indices, skyLights, torchLights, *numPositions, *numUvs, *numIds, *numIndices);
}

EMSCRIPTEN_KEEPALIVE void encodeGeometry(float *positions, float *uvs, float *colors, unsigned int *indices, unsigned int numPositions, unsigned int numUvs, unsigned int numColors, unsigned int numIndices, char *nameData, unsigned int nameSize, unsigned int transparent, unsigned int vegetation, unsigned int animal, unsigned char *data, unsigned int *size) {
  doEncodeGeometry(positions, uvs, colors, indices, numPositions, numUvs, numColors, numIndices, nameData, nameSize, transparent, vegetation, animal, data, *size);
}

EMSCRIPTEN_KEEPALIVE void doFree(void *ptr) {
  free(ptr);
}

}
