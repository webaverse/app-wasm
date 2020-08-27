#ifndef MARCH_H
#define MARCH_H

#include <vector>

enum class PEEK_FACES : int {
  FRONT = 1,
  BACK,
  LEFT,
  RIGHT,
  TOP,
  BOTTOM,
};
extern int PEEK_FACE_INDICES[];

class Tracker;
class Subparcel;

// void marchingCubes(int dims[3], float *potential, int shift[3], int indexOffset, float *positions, unsigned int *faces, unsigned int &positionIndex, unsigned int &faceIndex);
void marchingCubes2(float meshId, int dims[3], float *potential, unsigned char *biomes, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float *positions, float *normals, float *uvs, /*float *barycentrics,*/ unsigned char *aos, float *ids, unsigned char *skyLights, unsigned char *torchLights, unsigned int &positionIndex, unsigned int &normalIndex, unsigned int &uvIndex, /*unsigned int &barycentricIndex,*/ unsigned int &aoIndex, unsigned int &idIndex, unsigned int &skyLightsIndex, unsigned int &torchLightsIndex, unsigned int &numOpaquePositions, unsigned int &numTransparentPositions, unsigned char *peeks);
std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> doMine(Tracker *tracker, float *position, float delta);
std::pair<bool, std::vector<std::shared_ptr<Subparcel>>> doLight(Tracker *tracker, float *position, float delta);

#endif
