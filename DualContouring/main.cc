#include "main.h"
#include "octree.h"

class BufferManager
{
private:
    // we store the pointers to each data that we want in outputBuffer
    vector<int> outputBuffer;
    template <typename T>
    int *copyVectorToMemory(const vector<T> vector)
    {
        unsigned int size = vector.size() * sizeof(vector[0]);
        int *buffer = (int *)malloc(size);
        memcpy(buffer, &vector.front(), size);
        return buffer;
    }

public:
    template <typename T>
    void appendVector(const vector<T> vector)
    {
        outputBuffer.push_back((intptr_t)copyVectorToMemory(vector));
    }

    template <typename T>
    void appendVectorSize(const vector<T> vector)
    {
        outputBuffer.push_back(vector.size());
    }
    // exporting the output buffer
    int *getOutputBuffer()
    {
        const int size = outputBuffer.size();
        int *output = (int *)malloc(size * sizeof(int));
        for (size_t i = 0; i < size; i++)
        {
            output[i] = outputBuffer[i];
        }
        return output;
    }
};

namespace DualContouring
{
    int *getOutputBuffer(PositionBuffer &positions, NormalBuffer &normals, IndexBuffer &indices)
    {

        // the order of appending data to the buffer is important,
        // we will read the data by the same order in javascript
        BufferManager bufferController;

        bufferController.appendVectorSize(positions);
        bufferController.appendVector(positions);

        bufferController.appendVectorSize(normals);
        bufferController.appendVector(normals);

        bufferController.appendVectorSize(indices);
        bufferController.appendVector(indices);

        return bufferController.getOutputBuffer();
    }

    // storing the octree roots here for search
    unordered_map<uint64_t, OctreeNode *> rootsListHashMap;

    int *createChunk(float x, float y, float z, const float lod, int octreeSize)
    {
        PositionBuffer positions;
        NormalBuffer normals;
        IndexBuffer indices;

        OctreeNode *root = constructOctreeDownwards(vec3(-octreeSize / 2) + vec3(x, y, z), LodLevel(lod), octreeSize);

        addOctreeRootToHashMap(root, rootsListHashMap);

        generateMeshFromOctree(root, positions, normals, indices);

        return getOutputBuffer(positions, normals, indices);
    }

    int *createSeam(float x, float y, float z, int octreeSize)
    {
        PositionBuffer positions;
        NormalBuffer normals;
        IndexBuffer indices;

        OctreeNode *root = getOctreeRootFromHashMap(ivec3(-octreeSize / 2) + ivec3(x, y, z), rootsListHashMap);
        if (root == nullptr)
        {
            return getOutputBuffer(positions, normals, indices);
        }

        vector<OctreeNode *> seamNodes = findSeamNodes(root, rootsListHashMap, getOctreeRootFromHashMap);

        OctreeNode *seamRoot = constructOctreeUpwards(seamRoot, seamNodes, root->min, root->size * 2);

        generateMeshFromOctree(seamRoot, positions, normals, indices);

        return getOutputBuffer(positions, normals, indices);
    }
}