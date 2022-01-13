#include "cut.h"
#include <vector>

//using namespace Slic3r;

void cut(
  float *positions,
  unsigned int numPositions,
  float *normals,
  unsigned int numNormals,
  float *uvs,
  unsigned int numUvs,
  unsigned int *faces,
  unsigned int numFaces,

  float *position,
  float *quaternion,
  float *scale,

  float *outPositions,
  unsigned int *numOutPositions,
  float *outNormals,
  unsigned int *numOutNormals,
  float *outUvs,
  unsigned int *numOutUvs,
  unsigned int *outFaces,
  unsigned int *numOutFaces
) {

  float *coords = positions;
  float numPoints = numPositions / 3;
  float indices = numFaces / 3;

  std::vector<float> points1;
  std::vector<float> points2;

  std::vector<float> normals1;
  std::vector<float> normals2;

  std::vector<float> uvs1;
  std::vector<float> uvs2;

  // for(int i=0;i<numPositions;i++){
  //   points1.push_back(positions[i]);
  // }

  for(int i=0;i<numFaces;i++){
    
  }

  // outPositions = &points1[0]; // wrong
  memcpy(outPositions, &points1[0], numPositions*4);
  memcpy(outNormals, normals, numNormals*4);
  memcpy(outUvs, uvs, numUvs*4);
  memcpy(outFaces, faces, numFaces*4);

  numOutPositions[0] = numPositions;
  numOutNormals[0] = numNormals;
  numOutUvs[0] = numUvs;
  numOutFaces[0] = numFaces;

  numOutPositions[1] = 0;
  numOutNormals[1] = 0;
  numOutUvs[1] = 0;
  numOutFaces[1] = 0;

  // {
  //   numOutPositions[0] = numPositions;
  //   for(int i=0;i<numPositions;i++){
  //     outPositions[i] = positions[i];
  //   }

  //   numOutNormals[0] = numNormals;
  //   for(int i=0;i<numNormals;i++){
  //     outNormals[i] = normals[i];
  //   }

  //   numOutUvs[0] = numUvs;
  //   for(int i=0;i<numUvs;i++){
  //     outUvs[i] = uvs[i];
  //   }

  //   numOutFaces[0] = numFaces;
  //   for(int i=0;i<numFaces;i++){
  //     outFaces[i] = faces[i];
  //   }
  // }
  // {
  //   numOutPositions[1] = numPositions;
  //   for(int i=0;i<numPositions;i++){
  //     outPositions[numPositions + i] = positions[i]+2;
  //   }

  //   numOutNormals[1] = numNormals;
  //   for(int i=0;i<numNormals;i++){
  //     outNormals[numNormals + i] = normals[i];
  //   }

  //   numOutUvs[1] = numUvs;
  //   for(int i=0;i<numUvs;i++){
  //     outUvs[numUvs + i] = uvs[i];
  //   }

  //   numOutFaces[1] = numFaces;
  //   for(int i=0;i<numFaces;i++){
  //     outFaces[numFaces + i] = faces[i];
  //   }
  // }

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