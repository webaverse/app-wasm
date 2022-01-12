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
    // numOutPositions[0] = numPositions;
    // for(int i=0;i<numPositions;i++){
    //   outPositions[i] = positions[i];
    // }
    // numOutNormals[0] = numNormals;
    // for(int i=0;i<numNormals;i++){
    //   outNormals[i] = normals[i];
    // }

    // numOutFaces[0] = numFaces;
    // for(int i=0;i<numFaces;i++){
    //   outFaces[i] = faces[i];
    // }
    
    numOutPositions[0] = 9;
    outPositions[0] = positions[0];
    outPositions[1] = positions[1];
    outPositions[2] = positions[2];
    outPositions[3] = positions[3];
    outPositions[4] = positions[4];
    outPositions[5] = positions[5];
    outPositions[6] = positions[6];
    outPositions[7] = positions[7];
    outPositions[8] = positions[8];

    numOutNormals[0] = 9;
    outNormals[0] = normals[0];
    outNormals[1] = normals[1];
    outNormals[2] = normals[2];
    outNormals[3] = normals[3];
    outNormals[4] = normals[4];
    outNormals[5] = normals[5];
    outNormals[6] = normals[6];
    outNormals[7] = normals[7];
    outNormals[8] = normals[8];

    numOutFaces[0] = 3;
    outFaces[0] = faces[0];
    outFaces[1] = faces[1];
    outFaces[2] = faces[2];
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