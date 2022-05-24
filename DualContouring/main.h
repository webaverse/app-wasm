#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <vector>
#include <cstdint>
#include <ctime>
#include <string.h>
#include "octree.h"
#include "bufferManager.h"
#include "cachedNoise.h"
#include "vectorMath.h"

namespace DualContouring
{
    // class Chunk
    // {
    // public:
    //     vec3 getMin();
    // };
    void clearTemporaryChunkData();
    void clearChunkRoot(float x, float y, float z);
    void generateChunkData(float x, float y, float z);
    void setChunkLod(float x, float y, float z, const int lod);
    int *createChunkMesh(float x, float y, float z);
};

#endif // MAIN_H