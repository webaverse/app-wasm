#include "cut.h"
#include <vector>

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

  std::vector<float> points1;
  std::vector<float> points2;

  std::vector<float> normals1;
  std::vector<float> normals2;

  std::vector<float> uvs1;
  std::vector<float> uvs2;

  for(int i=0;i<facesCount;i++){
    unsigned int va = getVertexIndex(i, 0, indices);
    unsigned int vb = getVertexIndex(i, 1, indices);
    unsigned int vc = getVertexIndex(i, 2, indices);

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

  memcpy(outPositions, &points1[0], points1.size()*4);
  memcpy(outNormals, &normals1[0], normals1.size()*4);
  memcpy(outUvs, &uvs1[0], uvs1.size()*4);

  numOutPositions[0] = points1.size();
  numOutNormals[0] = normals1.size();
  numOutUvs[0] = uvs1.size();



  for(int i=0;i<facesCount/2;i++){
    unsigned int va = getVertexIndex(i, 0, indices);
    unsigned int vb = getVertexIndex(i, 1, indices);
    unsigned int vc = getVertexIndex(i, 2, indices);

    points2.push_back(coords[3 * va]);
    points2.push_back(coords[3 * va + 1]);
    points2.push_back(coords[3 * va + 2]);
    
    points2.push_back(coords[3 * vb]);
    points2.push_back(coords[3 * vb + 1]);
    points2.push_back(coords[3 * vb + 2]);
    
    points2.push_back(coords[3 * vc]);
    points2.push_back(coords[3 * vc + 1]);
    points2.push_back(coords[3 * vc + 2]);
    
    normals2.push_back(normals[3 * va]);
    normals2.push_back(normals[3 * va + 1]);
    normals2.push_back(normals[3 * va + 2]);
    
    normals2.push_back(normals[3 * vb]);
    normals2.push_back(normals[3 * vb + 1]);
    normals2.push_back(normals[3 * vb + 2]);
    
    normals2.push_back(normals[3 * vc]);
    normals2.push_back(normals[3 * vc + 1]);
    normals2.push_back(normals[3 * vc + 2]);
    
    uvs2.push_back(uvs[2 * va]);
    uvs2.push_back(uvs[2 * va + 1]);
    
    uvs2.push_back(uvs[2 * vb]);
    uvs2.push_back(uvs[2 * vb + 1]);
    
    uvs2.push_back(uvs[2 * vc]);
    uvs2.push_back(uvs[2 * vc + 1]);
  }

  for(int i=0;i<points2.size();i++){
    points2[i] += 1;
  }

  memcpy(outPositions + points1.size(), &points2[0], points2.size()*4);
  memcpy(outNormals + normals1.size(), &normals2[0], normals2.size()*4);
  memcpy(outUvs + uvs1.size(), &uvs2[0], uvs2.size()*4);

  numOutPositions[1] = points2.size();
  numOutNormals[1] = normals2.size();
  numOutUvs[1] = uvs2.size();

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