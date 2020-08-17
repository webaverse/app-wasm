#ifndef MARCH_H
#define MARCH_H

// void marchingCubes(int dims[3], float *potential, int shift[3], int indexOffset, float *positions, unsigned int *faces, unsigned int &positionIndex, unsigned int &faceIndex);
void marchingCubes2(int dims[3], float *potential, unsigned char *biomes, char *heightfield, unsigned char *lightfield, float shift[3], float scale[3], float *positions, float *normals, float *uvs, float *barycentrics, unsigned char *aos, unsigned int &positionIndex, unsigned int &normalIndex, unsigned int &uvIndex, unsigned int &barycentricIndex, unsigned int &aoIndex, unsigned char *skyLights, unsigned char *torchLights, unsigned int &numOpaquePositions, unsigned int &numTransparentPositions, unsigned char *peeks);

#endif
