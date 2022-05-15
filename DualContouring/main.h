#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <vector>
#include <cstdint>
#include <ctime>
#include <string.h>
#include "../glm/glm.hpp"
#include "octree.h"

namespace DualContouring
{
    // class Chunk
    // {
    // public:
    //     vec3 getMin();
    // };
    void destroyUselessOctrees();
    void generateChunkData(float x, float y, float z);
    void setChunkLod(float x, float y, float z, const float lod);
    int *generateChunkMesh(float x, float y, float z);
};

#endif // MAIN_H