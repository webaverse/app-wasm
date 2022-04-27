#include "main.h"
#include "octree.h"
// using namespace glm;

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
    vector<OctreeNode *> rootsList;

    OctreeNode *getOctreeRoot(ivec3 octreeMin)
    {
        auto octree = std::find_if(rootsList.begin(), rootsList.end(), [=](OctreeNode *node)
                                   { return node->min == octreeMin; });
        if (
            octree != rootsList.end())
        {
            // Found the octree
            return *octree;
        }
        else
        {
            // Did not found the octree
            return nullptr;
        }
    }

    OctreeNode *createOctree(vec3 origin, int octreeSize)
    {
        const int MAX_THRESHOLDS = 5;
        const float THRESHOLDS[MAX_THRESHOLDS] = {-1.f, 0.1f, 1.f, 10.f, 50.f};
        int thresholdIndex = -1;
        thresholdIndex = (thresholdIndex + 1) % MAX_THRESHOLDS;

        OctreeNode *root = nullptr;
        root = buildOctree(ivec3(-octreeSize / 2) + ivec3(origin), octreeSize, THRESHOLDS[thresholdIndex]);
        return root;
    }

    int *createChunk(float x, float y, float z)
    {
        PositionBuffer positions;
        NormalBuffer normals;
        IndexBuffer indices;

        OctreeNode *root = createOctree(vec3(x, y, z));
        rootsList.push_back(root);

        generateMeshFromOctree(root, positions, normals, indices);

        return getOutputBuffer(positions, normals, indices);
    }

    int *createSeam(float x, float y, float z)
    {
        PositionBuffer positions;
        NormalBuffer normals;
        IndexBuffer indices;

        OctreeNode *root = getOctreeRoot(-64 / 2 + ivec3(x, y, z));

        vector<OctreeNode *> seamNodes = findSeamNodes(root, getOctreeRoot);

        OctreeNode *seamRoot = constructOctreeUpwards(seamRoot, seamNodes, root->min, root->size * 2);
        generateMeshFromOctree(seamRoot, positions, normals, indices);
        return getOutputBuffer(positions, normals, indices);
    }
}

// int main()
// {
//     const ivec3 OFFSETS[8] =
//         {
//             ivec3(0, 0, 0), ivec3(1, 0, 0), ivec3(0, 0, 1), ivec3(1, 0, 1),
//             ivec3(0, 1, 0), ivec3(1, 1, 0), ivec3(0, 1, 1), ivec3(1, 1, 1)};

//     for (int i = 0; i < 4; i++)
//     {
//         vec3 pos = vec3(OFFSETS[i] * 64);
//         DualContouring::createChunk(pos.x, pos.y, pos.z);
//     }

//     cout << "\n"
//          << DualContouring::createSeam(0, 0, 0) << "\n"
//          << endl;

//     return 0;
// }