#include "cut.h"
#include <vector>

unsigned int getVertexIndex(int faceIdx, int vert, unsigned int *indices) {
  // vert = 0, 1 or 2.
  int idx = faceIdx * 3 + vert;
  // return indices ? indices[idx] : idx;
  return indices[idx];
}

// https://stackoverflow.com/a/4609795/3596736
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

float dot( float ax, float ay, float az, float bx, float by, float bz ) {

  return ax * bx + ay * by + az * bz;

}

float distanceToPoint( float x, float y, float z ) {

  // return this.normal.dot( point ) + this.constant;
  return dot( 1, 0, 0, x, y, z ) + 0;

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

    float *v0 = &coords[3 * va];
    float *v1 = &coords[3 * vb];
    float *v2 = &coords[3 * vc];

    float *n0 = &normals[3 * va];
    float *n1 = &normals[3 * vb];
    float *n2 = &normals[3 * vc];

    float *u0 = &uvs[2 * va];
    float *u1 = &uvs[2 * vb];
    float *u2 = &uvs[2 * vc];

    float d0 = distanceToPoint(*(v0), *(v0+1), *(v0+2));
    float d1 = distanceToPoint(*(v1), *(v1+1), *(v1+2));
    float d2 = distanceToPoint(*(v2), *(v2+1), *(v2+2));

    int sign0 = sgn(d0);
    int sign1 = sgn(d1);
    int sign2 = sgn(d2);

    if (sign0 == sign1 && sign1 == sign2 && sign2 == sign0) {
      if (sign0 == -1) {
        // points1.push(v0, v1, v2)
        points1.push_back(*(v0));
        points1.push_back(*(v0+1));
        points1.push_back(*(v0+2));
        points1.push_back(*(v1));
        points1.push_back(*(v1+1));
        points1.push_back(*(v1+2));
        points1.push_back(*(v2));
        points1.push_back(*(v2+1));
        points1.push_back(*(v2+2));
        
        // normals1.push_back(n0, n1, n2)
        normals1.push_back(*(n0));
        normals1.push_back(*(n0+1));
        normals1.push_back(*(n0+2));
        normals1.push_back(*(n1));
        normals1.push_back(*(n1+1));
        normals1.push_back(*(n1+2));
        normals1.push_back(*(n2));
        normals1.push_back(*(n2+1));
        normals1.push_back(*(n2+2));

        // uvs1.push_back(u0, u1, u2)
        uvs1.push_back(*(u0));
        uvs1.push_back(*(u0+1));
        uvs1.push_back(*(u1));
        uvs1.push_back(*(u1+1));
        uvs1.push_back(*(u2));
        uvs1.push_back(*(u2+1));
      } else if (sign0 == 1) {
        // points2.push(v0, v1, v2)
        points2.push_back(*(v0));
        points2.push_back(*(v0+1));
        points2.push_back(*(v0+2));
        points2.push_back(*(v1));
        points2.push_back(*(v1+1));
        points2.push_back(*(v1+2));
        points2.push_back(*(v2));
        points2.push_back(*(v2+1));
        points2.push_back(*(v2+2));
        
        // normals2.push_back(n0, n1, n2)
        normals2.push_back(*(n0));
        normals2.push_back(*(n0+1));
        normals2.push_back(*(n0+2));
        normals2.push_back(*(n1));
        normals2.push_back(*(n1+1));
        normals2.push_back(*(n1+2));
        normals2.push_back(*(n2));
        normals2.push_back(*(n2+1));
        normals2.push_back(*(n2+2));

        // uvs2.push_back(u0, u1, u2)
        uvs2.push_back(*(u0));
        uvs2.push_back(*(u0+1));
        uvs2.push_back(*(u1));
        uvs2.push_back(*(u1+1));
        uvs2.push_back(*(u2));
        uvs2.push_back(*(u2+1));
      }
    }
  }

  memcpy(outPositions, &points1[0], points1.size()*4);
  memcpy(outNormals, &normals1[0], normals1.size()*4);
  memcpy(outUvs, &uvs1[0], uvs1.size()*4);

  numOutPositions[0] = points1.size();
  numOutNormals[0] = normals1.size();
  numOutUvs[0] = uvs1.size();

  // part 2

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