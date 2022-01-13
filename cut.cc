#include "cut.h"
#include <vector>

//using namespace Slic3r;

unsigned int getVertexIndex(int faceIdx, int vert, unsigned int *indices) {
  // vert = 0, 1 or 2.
  int idx = faceIdx * 3 + vert;
  // return indices ? indices[idx] : idx;
  return indices[idx];
}

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

  unsigned int *indices = faces;

  float *coords = positions;
  unsigned int pointsCount = numPositions / 3;
  unsigned int facesCount = numFaces / 3;
  // float indices = numFaces / 3;

  std::vector<float> points1;
  std::vector<float> points2;

  std::vector<float> normals1;
  std::vector<float> normals2;

  std::vector<float> uvs1;
  std::vector<float> uvs2;

  // threepp::Vector3 _vector1{};

  // for(int i=0;i<numPositions/2;i++){
  //   points1.push_back(positions[i]);
  // }

  for(int i=0;i<facesCount;i++){
  // for(int i=0;i<1;i++){
    unsigned int va = getVertexIndex(i, 0, indices);
    unsigned int vb = getVertexIndex(i, 1, indices);
    unsigned int vc = getVertexIndex(i, 2, indices);

    // threepp::Vector3 v0(coords[3 * va], coords[3 * va + 1], coords[3 * va + 2]);
    // threepp::Vector3 v1(coords[3 * vb], coords[3 * vb + 1], coords[3 * vb + 2]);
    // threepp::Vector3 v2(coords[3 * vc], coords[3 * vc + 1], coords[3 * vc + 2]);
    
    points1.push_back(coords[3 * va]);
    points1.push_back(coords[3 * va + 1]);
    points1.push_back(coords[3 * va + 2]);
    
    points1.push_back(coords[3 * vb]);
    points1.push_back(coords[3 * vb + 1]);
    points1.push_back(coords[3 * vb + 2]);
    
    points1.push_back(coords[3 * vc]);
    points1.push_back(coords[3 * vc + 1]);
    points1.push_back(coords[3 * vc + 2]);
    
    normals1.push_back(normals[3 * va]);
    normals1.push_back(normals[3 * va + 1]);
    normals1.push_back(normals[3 * va + 2]);
    
    normals1.push_back(normals[3 * vb]);
    normals1.push_back(normals[3 * vb + 1]);
    normals1.push_back(normals[3 * vb + 2]);
    
    normals1.push_back(normals[3 * vc]);
    normals1.push_back(normals[3 * vc + 1]);
    normals1.push_back(normals[3 * vc + 2]);
    
    uvs1.push_back(uvs[2 * va]);
    uvs1.push_back(uvs[2 * va + 1]);
    
    uvs1.push_back(uvs[2 * vb]);
    uvs1.push_back(uvs[2 * vb + 1]);
    
    uvs1.push_back(uvs[2 * vc]);
    uvs1.push_back(uvs[2 * vc + 1]);
  }

  // outPositions = &points1[0]; // wrong
  memcpy(outPositions, &points1[0], points1.size()*4);
  memcpy(outNormals, &normals1[0], normals1.size()*4);
  memcpy(outUvs, &uvs1[0], uvs1.size()*4);
  memcpy(outFaces, faces, numFaces*4);

  numOutPositions[0] = points1.size();
  numOutNormals[0] = normals1.size();
  numOutUvs[0] = uvs1.size();
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