#ifndef MESH_H
#define MESH_H

#include "vectorMath.h"

#include <vector>

typedef std::vector<vm::vec3> PositionBuffer;
typedef std::vector<vm::vec3> NormalBuffer;
typedef std::vector<vm::ivec4> BiomeBuffer;
typedef std::vector<vm::vec4> BiomeWeightBuffer;
typedef std::vector<int> IndexBuffer;

class VertexBuffer {
public:
    PositionBuffer positions;
    NormalBuffer normals;
    BiomeBuffer biomes;
    BiomeWeightBuffer biomesWeights;
    IndexBuffer indices;

    uint8_t *getBuffer() const;
};

#endif // MESH_H
