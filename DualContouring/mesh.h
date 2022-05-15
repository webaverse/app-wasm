#ifndef MESH_H
#define MESH_H

#include <vector>
#include "../glm/glm.hpp"

typedef std::vector<glm::vec3> PositionBuffer;
typedef std::vector<glm::vec3> NormalBuffer;
typedef std::vector<glm::vec4> BiomeBuffer;
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
