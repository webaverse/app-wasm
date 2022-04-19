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
    int *createChunk()
    {

        const int MAX_THRESHOLDS = 5;
        const float THRESHOLDS[MAX_THRESHOLDS] = {-1.f, 0.1f, 1.f, 10.f, 50.f};
        int thresholdIndex = -1;

        OctreeNode *root = nullptr;

        // octreeSize must be a power of two!
        const int octreeSize = 64;

        float rotateX = -60.f, rotateY = 0.f;
        float distance = 100.f;
        bool drawWireframe = false;
        bool refreshMesh = true;

        thresholdIndex = (thresholdIndex + 1) % MAX_THRESHOLDS;

        PositionBuffer positions;
        NormalBuffer normals;
        IndexBuffer indices;

        root = BuildOctree(glm::ivec3(-octreeSize / 2), octreeSize, THRESHOLDS[thresholdIndex]);
        GenerateMeshFromOctree(root, positions, normals, indices);

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
}

int main()
{

    cout << "\n"
         << DualContouring::createChunk() << "\n"
         << endl;

    return 0;
}