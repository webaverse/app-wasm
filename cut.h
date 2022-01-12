// #define CSGJS_HEADER_ONLY
// #include "csgjs.cpp"
// #include "earcut.hpp"
#include "TriangleMesh.hpp"

void cut(
  float *positions,
  unsigned int numPositions,
  float *normals,
  unsigned int numNormals,
  unsigned int *faces,
  unsigned int numFaces,
  float *position,
  float *quaternion,
  float *scale,
  float *outPositions,
  unsigned int *numOutPositions,
  float *outNormals,
  unsigned int *numOutNormals,
  unsigned int *outFaces,
  unsigned int *numOutFaces
);