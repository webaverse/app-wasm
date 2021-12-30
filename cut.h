// #define CSGJS_HEADER_ONLY
// #include "csgjs.cpp"
// #include "earcut.hpp"
#include "TriangleMesh.hpp"

void cut(
  float *positions,
  unsigned int numPositions,
  unsigned int *faces,
  unsigned int numFaces,
  float *position,
  float *quaternion,
  float *scale,
  float *outPositions,
  unsigned int *numOutPositions,
  unsigned int *outFaces,
  unsigned int *numOutFaces
);