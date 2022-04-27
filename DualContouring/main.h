#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <vector>
#include <cstdint>
#include <ctime>
#include <string.h>
#include "../glm/glm.hpp"
#include "octree.h"

using namespace std;
using namespace glm;

namespace DualContouring
{
    // class Chunk
    // {
    // public:
    //     vec3 getMin();
    // };
    OctreeNode *createOctree(vec3 origin, int octreeSize = 64);
    int *createChunk(float x, float y, float z);
    int *createSeam(float x, float y, float z);
};

#endif // MAIN_H