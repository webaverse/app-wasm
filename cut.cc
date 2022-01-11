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

    // // position

    // numOutPositions[0] = 9;

    // outPositions[0] = -1;
    // outPositions[1] = 0;
    // outPositions[2] = 0;

    // outPositions[3] = 0;
    // outPositions[4] = 3;
    // outPositions[5] = 0;

    // outPositions[6] = 2;
    // outPositions[7] = 0;
    // outPositions[8] = 0;

    // // face

    // numOutFaces[0] = 3;
    
    // outFaces[0] = 0;
    // outFaces[1] = 1;
    // outFaces[2] = 2;


    // // position

    // numOutPositions[0] = 9;

    // outPositions[0] = positions[0];
    // outPositions[1] = positions[1];
    // outPositions[2] = positions[2];

    // outPositions[3] = positions[3];
    // outPositions[4] = positions[4];
    // outPositions[5] = positions[5];

    // outPositions[6] = positions[6];
    // outPositions[7] = positions[7];
    // outPositions[8] = positions[8];

    // // face

    // numOutFaces[0] = 3;
    
    // outFaces[0] = faces[0];
    // outFaces[1] = faces[1];
    // outFaces[2] = faces[2];


    // position
    Pointf3 * points = (Pointf3*)positions;

    numOutPositions[0] = 9;

    outPositions[0] = points[0].x;
    outPositions[1] = points[0].y;
    outPositions[2] = points[0].z;

    outPositions[3] = points[1].x;
    outPositions[4] = points[1].y;
    outPositions[5] = points[1].z;

    outPositions[6] = points[2].x;
    outPositions[7] = points[2].y;
    outPositions[8] = points[2].z;

    // face

    numOutFaces[0] = 3;
    
    outFaces[0] = faces[0];
    outFaces[1] = faces[1];
    outFaces[2] = faces[2];
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