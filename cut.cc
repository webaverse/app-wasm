#include "cut.h"
#include <vector>

float planeNormalX = 1;
float planeNormalY = 0;
float planeNormalZ = 0;
float planeConstant = 0;

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

// vector3
float dot( float ax, float ay, float az, float bx, float by, float bz ) {

  return ax * bx + ay * by + az * bz;

}

// vector3
float lengthSq( float x, float y, float z ) {

  return x * x + y * y + z * z;

}

// plane
float distanceToPoint( float x, float y, float z ) {

  // return this.normal.dot( point ) + this.constant;
  return dot( planeNormalX, planeNormalY, planeNormalZ, x, y, z ) + planeConstant;

}

// plane
void intersectLine( 
  float *targetX, float *targetY, float *targetZ, 
  float lineStartX, float lineStartY, float lineStartZ, 
  float lineEndX, float lineEndY, float lineEndZ
) {

  float directionX = lineEndX - lineStartX;
  float directionY = lineEndY - lineStartY;
  float directionZ = lineEndZ - lineStartZ;

  float denominator = dot( planeNormalX, planeNormalY, planeNormalZ, directionX, directionY, directionZ );

  if ( denominator == 0 ) {

    // line is coplanar, return origin
    // if ( this.distanceToPoint( line.start ) === 0 ) {
    if ( distanceToPoint( lineStartX, lineStartY, lineStartZ ) == 0) {

      // return target.copy( line.start );
      *targetX = lineStartX;
      *targetY = lineStartY;
      *targetZ = lineStartZ;
      return;

    }

  }

  // const t = - ( line.start.dot( this.normal ) + this.constant ) / denominator;
  float t = - ( dot(lineStartX, lineStartY, lineStartZ, planeNormalX, planeNormalY, planeNormalZ) + planeConstant ) / denominator;

  if ( t < 0 || t > 1 ) {

    return;

  }

  // return target.copy( direction ).multiplyScalar( t ).add( line.start );
  *targetX = directionX * t + lineStartX;
  *targetY = directionY * t + lineStartY;
  *targetZ = directionZ * t + lineStartZ;
  return;

}

void getIntersectNode(
  float *vIX, float *vIY, float *vIZ,
  float *nIX, float *nIY, float *nIZ,
  float *uIX, float *uIY,
  float v0X, float v0Y, float v0Z,
  float v1X, float v1Y, float v1Z,
  float n0X, float n0Y, float n0Z,
  float n1X, float n1Y, float n1Z,
  float u0X, float u0Y,
  float u1X, float u1Y
  // v0, v1, n0, n1, u0, u1
) {
  // this.tempLine1.start.copy(v0)
  // this.tempLine1.end.copy(v1)
  // let vI = new THREE.Vector3()
  // vI = this.localPlane.intersectLine(this.tempLine1, vI)
  intersectLine(
    vIX, vIY, vIZ,
    v0X, v0Y, v0Z,
    v1X, v1Y, v1Z
  );

  // let total = this.tempVector3.subVectors(v1, v0).lengthSq()
  float total = lengthSq( v1X-v0X, v1Y-v0Y, v1Z-v0Z );
  // let part = this.tempVector3.subVectors(vI, v0).lengthSq()
  float part = lengthSq( *vIX-v0X, *vIY-v0Y, *vIZ-v0Z );
  // let ratio = Math.sqrt(part / total)
  float ratio = sqrt( part / total );

  // let normalPart = this.tempVector3.subVectors(n1, n0).multiplyScalar(ratio)
  float normalPartX = ( n1X - n0X ) * ratio;
  float normalPartY = ( n1Y - n0Y ) * ratio;
  float normalPartZ = ( n1Z - n0Z ) * ratio;
  // let nI = new THREE.Vector3().copy(n0).add(normalPart)
  *nIX = n0X + normalPartX;
  *nIY = n0Y + normalPartY;
  *nIZ = n0Z + normalPartZ;

  // let uvPart = this.tempVector2.subVectors(u1, u0).multiplyScalar(ratio)
  float uvPartX = ( u1X - u0X ) * ratio;
  float uvPartY = ( u1Y - u0Y ) * ratio;
  // let uI = new THREE.Vector2().copy(u0).add(uvPart)
  *uIX = u0X + uvPartX;
  *uIY = u0Y + uvPartY;

  // return { vI, nI, uI }
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

    float v0X = coords[3 * va];
    float v1X = coords[3 * vb];
    float v2X = coords[3 * vc];
    float v0Y = coords[3 * va + 1];
    float v1Y = coords[3 * vb + 1];
    float v2Y = coords[3 * vc + 1];
    float v0Z = coords[3 * va + 2];
    float v1Z = coords[3 * vb + 2];
    float v2Z = coords[3 * vc + 2];

    float n0X = normals[3 * va];
    float n1X = normals[3 * vb];
    float n2X = normals[3 * vc];
    float n0Y = normals[3 * va + 1];
    float n1Y = normals[3 * vb + 1];
    float n2Y = normals[3 * vc + 1];
    float n0Z = normals[3 * va + 2];
    float n1Z = normals[3 * vb + 2];
    float n2Z = normals[3 * vc + 2];

    float u0X = uvs[2 * va];
    float u1X = uvs[2 * vb];
    float u2X = uvs[2 * vc];
    float u0Y = uvs[2 * va + 1];
    float u1Y = uvs[2 * vb + 1];
    float u2Y = uvs[2 * vc + 1];

    float d0 = distanceToPoint(v0X, v0Y, v0Z);
    float d1 = distanceToPoint(v1X, v1Y, v1Z);
    float d2 = distanceToPoint(v2X, v2Y, v2Z);

    int sign0 = sgn(d0);
    int sign1 = sgn(d1);
    int sign2 = sgn(d2);

    if (sign0 == sign1 && sign1 == sign2 && sign2 == sign0) {
      if (sign0 == -1) {
        // points1.push(v0, v1, v2)
        points1.push_back(v0X);
        points1.push_back(v0Y);
        points1.push_back(v0Z);
        points1.push_back(v1X);
        points1.push_back(v1Y);
        points1.push_back(v1Z);
        points1.push_back(v2X);
        points1.push_back(v2Y);
        points1.push_back(v2Z);
        // normals1.push_back(n0, n1, n2)
        normals1.push_back(n0X);
        normals1.push_back(n0Y);
        normals1.push_back(n0Z);
        normals1.push_back(n1X);
        normals1.push_back(n1Y);
        normals1.push_back(n1Z);
        normals1.push_back(n2X);
        normals1.push_back(n2Y);
        normals1.push_back(n2Z);
        // uvs1.push_back(u0, u1, u2)
        uvs1.push_back(u0X);
        uvs1.push_back(u0Y);
        uvs1.push_back(u1X);
        uvs1.push_back(u1Y);
        uvs1.push_back(u2X);
        uvs1.push_back(u2Y);
      } else if (sign0 == 1) {
        // points2.push(v0, v1, v2)
        points2.push_back(v0X);
        points2.push_back(v0Y);
        points2.push_back(v0Z);
        points2.push_back(v1X);
        points2.push_back(v1Y);
        points2.push_back(v1Z);
        points2.push_back(v2X);
        points2.push_back(v2Y);
        points2.push_back(v2Z);
        // normals2.push_back(n0, n1, n2)
        normals2.push_back(n0X);
        normals2.push_back(n0Y);
        normals2.push_back(n0Z);
        normals2.push_back(n1X);
        normals2.push_back(n1Y);
        normals2.push_back(n1Z);
        normals2.push_back(n2X);
        normals2.push_back(n2Y);
        normals2.push_back(n2Z);
        // uvs2.push_back(u0, u1, u2)
        uvs2.push_back(u0X);
        uvs2.push_back(u0Y);
        uvs2.push_back(u1X);
        uvs2.push_back(u1Y);
        uvs2.push_back(u2X);
        uvs2.push_back(u2Y);
      }
    } else if (sign0 == sign1) {
        if (sign0 == -1) {
          if (sign2 == 1) {
            // let { vI: vI0, nI: nI0, uI: uI0 } = getIntersectNode(v0, v2, n0, n1, u0, u2)
            float vI0X, vI0Y, vI0Z, nI0X, nI0Y, nI0Z, uI0X, uI0Y;
            getIntersectNode(
              &vI0X, &vI0Y, &vI0Z, &nI0X, &nI0Y, &nI0Z, &uI0X, &uI0Y,
              v0X, v0Y, v0Z,
              v2X, v2Y, v2Z,
              n0X, n0Y, n0Z,
              n1X, n1Y, n1Z,
              u0X, u0Y,
              u2X, u2Y
            );
            // let { vI: vI1, nI: nI1, uI: uI1 } = getIntersectNode(v1, v2, n1, n2, u1, u2)
            float vI1X, vI1Y, vI1Z, nI1X, nI1Y, nI1Z, uI1X, uI1Y;
            getIntersectNode(
              &vI1X, &vI1Y, &vI1Z, &nI1X, &nI1Y, &nI1Z, &uI1X, &uI1Y,
              v1X, v1Y, v1Z,
              v2X, v2Y, v2Z,
              n1X, n1Y, n1Z,
              n2X, n2Y, n2Z,
              u1X, u1Y,
              u2X, u2Y
            );
            // points1.push(v0, vI1, vI0)
            points1.push_back(v0X);
            points1.push_back(v0Y);
            points1.push_back(v0Z);
            points1.push_back(vI1X);
            points1.push_back(vI1Y);
            points1.push_back(vI1Z);
            points1.push_back(vI0X);
            points1.push_back(vI0Y);
            points1.push_back(vI0Z);
            // normals1.push(n0, nI1, nI0)
            normals1.push_back(n0X);
            normals1.push_back(n0Y);
            normals1.push_back(n0Z);
            normals1.push_back(vI1X);
            normals1.push_back(vI1Y);
            normals1.push_back(vI1Z);
            normals1.push_back(vI0X);
            normals1.push_back(vI0Y);
            normals1.push_back(vI0Z);
            // uvs1.push(u0, uI1, uI0)
            uvs1.push_back(u0X);
            uvs1.push_back(u0Y);
            uvs1.push_back(uI1X);
            uvs1.push_back(uI1Y);
            uvs1.push_back(uI0X);
            uvs1.push_back(uI0Y);
            // points1.push(v0, v1, vI1)
            points1.push_back(v0X);
            points1.push_back(v0Y);
            points1.push_back(v0Z);
            points1.push_back(v1X);
            points1.push_back(v1Y);
            points1.push_back(v1Z);
            points1.push_back(vI1X);
            points1.push_back(vI1Y);
            points1.push_back(vI1Z);
            // normals1.push(n0, n1, nI1)
            normals1.push_back(n0X);
            normals1.push_back(n0Y);
            normals1.push_back(n0Z);
            normals1.push_back(n1X);
            normals1.push_back(n1Y);
            normals1.push_back(n1Z);
            normals1.push_back(nI1X);
            normals1.push_back(nI1Y);
            normals1.push_back(nI1Z);
            // uvs1.push(u0, u1, uI1)
            uvs1.push_back(u0X);
            uvs1.push_back(u0Y);
            uvs1.push_back(u1X);
            uvs1.push_back(u1Y);
            uvs1.push_back(uI1X);
            uvs1.push_back(uI1Y);
            // points2.push(v2, vI0, vI1)
            points2.push_back(v2X);
            points2.push_back(v2Y);
            points2.push_back(v2Z);
            points2.push_back(vI0X);
            points2.push_back(vI0Y);
            points2.push_back(vI0Z);
            points2.push_back(vI1X);
            points2.push_back(vI1Y);
            points2.push_back(vI1Z);
            // normals2.push(n2, nI0, nI1)
            normals2.push_back(n2X);
            normals2.push_back(n2Y);
            normals2.push_back(n2Z);
            normals2.push_back(nI0X);
            normals2.push_back(nI0Y);
            normals2.push_back(nI0Z);
            normals2.push_back(nI1X);
            normals2.push_back(nI1Y);
            normals2.push_back(nI1Z);
            // uvs2.push(u2, uI0, uI1)
            uvs2.push_back(u2X);
            uvs2.push_back(u2Y);
            uvs2.push_back(uI0X);
            uvs2.push_back(uI0Y);
            uvs2.push_back(uI1X);
            uvs2.push_back(uI1Y);
          } else if (sign2 == 0) {
            // points1.push(v0, v1, v2)
            points1.push_back(v0X);
            points1.push_back(v0Y);
            points1.push_back(v0Z);
            points1.push_back(v1X);
            points1.push_back(v1Y);
            points1.push_back(v1Z);
            points1.push_back(v2X);
            points1.push_back(v2Y);
            points1.push_back(v2Z);
            // normals1.push(n0, n1, n2)
            normals1.push_back(n0X);
            normals1.push_back(n0Y);
            normals1.push_back(n0Z);
            normals1.push_back(n1X);
            normals1.push_back(n1Y);
            normals1.push_back(n1Z);
            normals1.push_back(n2X);
            normals1.push_back(n2Y);
            normals1.push_back(n2Z);
            // uvs1.push(u0, u1, u2)
            uvs1.push_back(u0X);
            uvs1.push_back(u0Y);
            uvs1.push_back(u1X);
            uvs1.push_back(u1Y);
            uvs1.push_back(u2X);
            uvs1.push_back(u2Y);
          }
        } else if (sign0 == 1) {
          // if (sign2 == -1) {
          //   let { vI: vI0, nI: nI0, uI: uI0 } = getIntersectNode(v0, v2, n0, n2, u0, u2)
          //   let { vI: vI1, nI: nI1, uI: uI1 } = getIntersectNode(v1, v2, n1, n2, u1, u2)
          //   points2.push(v0, vI1, vI0)
          //   normals2.push(n0, nI1, nI0)
          //   uvs2.push(u0, uI1, uI0)
          //   points2.push(v0, v1, vI1)
          //   normals2.push(n0, n1, nI1)
          //   uvs2.push(u0, u1, uI1)
          //   points1.push(v2, vI0, vI1)
          //   normals1.push(n2, nI0, nI1)
          //   uvs1.push(u2, uI0, uI1)
          // } else if (sign2 == 0) {
          //   points2.push(v0, v1, v2)
          //   normals2.push(n0, n1, n2)
          //   uvs2.push(u0, u1, u2)
          // }
        } else if (sign0 == 0) {
          // if (sign2 == -1) {
          //   points1.push(v0, v1, v2)
          //   normals1.push(n0, n1, n2)
          //   uvs1.push(u0, u1, u2)
          // } else if (sign2 == 1) {
          //   points2.push(v0, v1, v2)
          //   normals2.push(n0, n1, n2)
          //   uvs2.push(u0, u1, u2)
          // }
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