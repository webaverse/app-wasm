#ifndef OCTREE_H
#define OCTREE_H

#include "vectorMath.h"
#define _USE_MATH_DEFINES

#include "qef.h"
#include "mesh.h"
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <stdint.h>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
// #include "../vm/glm.hpp"
#include "cachedNoise.h"
#include "chunkDamageBuffer.h"

enum OctreeNodeType
{
    Node_None,
    Node_Internal,
    Node_Psuedo,
    Node_Leaf
};

enum LodLevel
{
    Lod0 = 1,
    Lod1 = 2,
    Lod2 = 4,
    Lod3 = 8
};

struct OctreeDrawInfo
{
    OctreeDrawInfo()
        : index(-1), corners(0)
    {
    }

    int index;
    int corners;
    vm::ivec4 biome;
    vm::vec4 biomeWeights;
    vm::vec3 position;
    vm::vec3 averageNormal;
    svd::QefData qef;
};

class OctreeNode
{
public:
    OctreeNode()
        : type(Node_None), min(0, 0, 0), size(0), drawInfo(nullptr), lod(Lod0)
    {
        for (int i = 0; i < 8; i++)
        {
            children[i] = nullptr;
        }
    }

    OctreeNode(const OctreeNodeType _type)
        : type(_type), min(0, 0, 0), size(0), drawInfo(nullptr), lod(Lod0)
    {
        for (int i = 0; i < 8; i++)
        {
            children[i] = nullptr;
        }
    }

    OctreeNodeType type;
    vm::ivec3 min;
    int size;
    LodLevel lod;
    OctreeNode *children[8];
    OctreeDrawInfo *drawInfo;
};

OctreeNode *createChunkWithLod(OctreeNode *chunkRoot);
OctreeNode *getChunkRootFromHashMap(vm::ivec3 octreeMin, std::unordered_map<uint64_t, OctreeNode *> &hashMap);
void removeOctreeFromHashMap(vm::ivec3 octreeMin, std::unordered_map<uint64_t, OctreeNode *> &hashMap);
void addChunkRootToHashMap(OctreeNode *root, std::unordered_map<uint64_t, OctreeNode *> &hashMap);
uint64_t hashOctreeMin(const vm::ivec3 &min);

std::vector<OctreeNode *> findSeamNodes(OctreeNode *root, std::vector<OctreeNode *>(&neighbouringChunks), std::unordered_map<uint64_t, OctreeNode *> hashMap, OctreeNode *(*getChunkRootFromHashMap)(vm::ivec3, std::unordered_map<uint64_t, OctreeNode *> &));
OctreeNode *constructOctreeUpwards(
    OctreeNode *octree,
    const std::vector<OctreeNode *> &inputNodes,
    const vm::ivec3 &rootMin,
    const int rootNodeSize);

OctreeNode *constructOctreeDownwards(const vm::ivec3 &min, const int size, CachedNoise &chunkNoise, ChunkDamageBuffer &damageBuffer);
OctreeNode *switchChunkLod(OctreeNode *node, LodLevel lod);
void destroyOctree(OctreeNode *node);
void generateMeshFromOctree(OctreeNode *node, bool isSeam, VertexBuffer &vertexBuffer);

void addNodesToVector(OctreeNode *node, std::vector<OctreeNode *> &vector);

#endif // OCTREE_H
