#ifndef MESH_H
#define MESH_H

#include "vectorMath.h"

#include <vector>

typedef std::vector<vm::vec3> PositionBuffer;
typedef std::vector<vm::vec3> NormalBuffer;
typedef std::vector<vm::vec4> BiomeBuffer;
typedef std::vector<int> IndexBuffer;

struct VertexBuffer
{
    PositionBuffer positions;
    NormalBuffer normals;
    IndexBuffer indices;
    BiomeBuffer biomes;
    BiomeBuffer biomesWeights;
};

#endif // MESH_H
