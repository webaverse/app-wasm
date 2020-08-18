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

EMSCRIPTEN_KEEPALIVE void getAnimalGeometry(GeometrySet *geometrySet, unsigned int hash, float **positions, unsigned char **colors, unsigned int **indices, float **heads, float **legs, unsigned int *numPositions, unsigned int *numColors, unsigned int *numIndices, unsigned int *numHeads, unsigned int *numLegs, float *headPivot, float *aabb) {
  doGetAnimalGeometry(geometrySet, hash, positions, colors, indices, heads, legs, *numPositions, *numColors, *numIndices, *numHeads, *numLegs, headPivot, aabb);
}

EMSCRIPTEN_KEEPALIVE void marchObjects(GeometrySet *geometrySet, int x, int y, int z, MarchObject *marchObjects, unsigned int numMarchObjects, SubparcelObject *subparcelObjects, unsigned int numSubparcelObjects, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int *numPositions, unsigned int *numUvs, unsigned int *numIds, unsigned int *numIndices, unsigned int *numSkyLights, unsigned int *numTorchLights) {
  doMarchObjects(geometrySet, x, y, z, marchObjects, numMarchObjects, subparcelObjects, numSubparcelObjects, positions, uvs, ids, indices, skyLights, torchLights, *numPositions, *numUvs, *numIds, *numIndices, *numSkyLights, *numTorchLights);
}

EMSCRIPTEN_KEEPALIVE void doFree(void *ptr) {
  free(ptr);
}

}