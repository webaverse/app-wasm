#include "cut.h"

//using namespace Slic3r;

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
) {

  {
    numOutPositions[0] = numPositions;
    for(int i=0;i<numPositions;i++){
      outPositions[i] = positions[i];
    }
    numOutNormals[0] = numNormals;
    for(int i=0;i<numNormals;i++){
      outNormals[i] = normals[i];
    }

    numOutFaces[0] = numFaces;
    for(int i=0;i<numFaces;i++){
      outFaces[i] = faces[i];
    }
  }
  {
    numOutPositions[1] = 0;
    numOutNormals[1] = 0;
    numOutFaces[1] = 0;
  }

  /* csgjs_plane plane;
  plane.normal = csgjs_vector(0, 1, 0);
  plane.w = 0.5;

  csgjs_polygon polygon;

  std::vector<csgjs_polygon> coplanarFront;
  std::vector<csgjs_polygon> coplanarBack;
  std::vector<csgjs_polygon> front;
  std::vector<csgjs_polygon> back;
  plane.splitPolygon(polygon, coplanarFront, coplanarBack, front, back); */
}