#ifndef OCTREE_H
#define OCTREE_H

#include "qef.h"
#include "mesh.h"
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <stdint.h>
#include <unordered_map>
#include <unordered_set>
#include "../glm/glm.hpp"

using namespace glm;
using namespace std;

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
    vec3 position;
    vec3 averageNormal;
    svd::QefData qef;
};

class OctreeNode
{
public:
    OctreeNode()
        : type(Node_None), min(0, 0, 0), size(0), drawInfo(nullptr)
    {
        for (int i = 0; i < 8; i++)
        {
            children[i] = nullptr;
        }
    }

    OctreeNode(const OctreeNodeType _type)
        : type(_type), min(0, 0, 0), size(0), drawInfo(nullptr)
    {
        for (int i = 0; i < 8; i++)
        {
            children[i] = nullptr;
        }
    }

    OctreeNodeType type;
    ivec3 min;
    int size;
    OctreeNode *children[8];
    OctreeDrawInfo *drawInfo;
};

OctreeNode *getOctreeRootFromHashMap(ivec3 octreeMin, unordered_map<uint64_t, OctreeNode *> &hashMap);
void addOctreeRootToHashMap(OctreeNode *root, unordered_map<uint64_t, OctreeNode *> &hashMap);
uint64_t hashOctreeMin(const ivec3 &min);

vector<OctreeNode *> findSeamNodes(OctreeNode *root, unordered_map<uint64_t, OctreeNode *> hashMap, OctreeNode *(*getOctreeRootFromHashMap)(ivec3, unordered_map<uint64_t, OctreeNode *> &));
OctreeNode *constructOctreeUpwards(
    OctreeNode *octree,
    const std::vector<OctreeNode *> &inputNodes,
    const glm::ivec3 &rootMin,
    const int rootNodeSize);

OctreeNode *constructOctreeDownwards(const vec3 &min, LodLevel lod, const int size);
void destroyOctree(OctreeNode *node);
void generateMeshFromOctree(OctreeNode *node, PositionBuffer &positionBuffer, NormalBuffer &normalBuffer, IndexBuffer &indexBuffer);

#endif // OCTREE_H
