#include "cut.h"

//using namespace Slic3r;

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
) {

  {
    // numOutPositions[0] = numPositions;
    // for (int i = 0; i < numPositions; i++) {
    //   outPositions[i] = positions[i];
    // }

    // // memcpy(outFaces, faces, numFaces*sizeof(faces[0]));
    // numOutFaces[0] = numFaces;
    // for (int i = 0; i < numFaces; i++) {
    //   outFaces[i] = faces[i];
    // }

    // position

    numOutPositions[0] = 9;

    outPositions[0] = -1;
    outPositions[1] = 0;
    outPositions[2] = 0;

    outPositions[3] = 0;
    outPositions[4] = 3;
    outPositions[5] = 0;

    outPositions[6] = 2;
    outPositions[7] = 0;
    outPositions[8] = 0;

    // face

    numOutFaces[0] = 3;
    
    outFaces[0] = 0;
    outFaces[1] = 1;
    outFaces[2] = 2;
  }
  {
    numOutPositions[1] = 0;
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