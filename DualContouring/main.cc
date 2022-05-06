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

    // chunk settings
    int chunkSize = 64;

    // storing the octrees that we would delete after mesh construction
    vector<OctreeNode *> destroyableNodesList;

    // storing the octree roots here for search
    unordered_map<uint64_t, OctreeNode *> chunksListHashMap;

    void generateChunkData(float x, float y, float z)
    {
        OctreeNode *chunk = constructOctreeDownwards(ivec3(-chunkSize / 2) + ivec3(x, y, z), chunkSize);
        addChunkRootToHashMap(chunk, chunksListHashMap);
    }

    void setChunkLod(float x, float y, float z, const float lod)
    {
        OctreeNode *chunkRoot = getChunkRootFromHashMap(ivec3(-chunkSize / 2) + ivec3(x, y, z), chunksListHashMap);
        if (chunkRoot == nullptr)
        {
            // cout << "NULL :/ for no reason" << endl;
            return;
        }
        chunkRoot->lod = LodLevel(lod);
    }

    void addNodesToVector(OctreeNode *node, vector<OctreeNode *> &vector)
    {
        if (!node)
        {
            return;
        }
        for (int i = 0; i < 8; i++)
        {
            addNodesToVector(node->children[i], vector);
        }
        vector.push_back(node);
    }

    void destroyUselessOctrees()
    {
        vector<OctreeNode *> nodesList;
        // nodesList.reserve(50000);
        for (int i = 0; i < destroyableNodesList.size(); i++)
        {
            addNodesToVector(destroyableNodesList[i], nodesList);
        }
        // sort and remove duplicates
        sort(nodesList.begin(), nodesList.end());
        nodesList.erase(unique(nodesList.begin(), nodesList.end()), nodesList.end());
        // cout << nodesList.size() << endl;
        for (int i = 0; i < nodesList.size(); i++)
        {
            const OctreeNode *node = nodesList[i];
            if (node->drawInfo)
            {
                delete node->drawInfo;
            }

            // cout << "DELETE NODE :" << node << endl;
            delete node;
        }
        destroyableNodesList.clear();
    }

    int *generateChunkMesh(float x, float y, float z)
    {
        PositionBuffer positions;
        NormalBuffer normals;
        IndexBuffer indices;

        OctreeNode *chunkRoot = getChunkRootFromHashMap(ivec3(-chunkSize / 2) + ivec3(x, y, z), chunksListHashMap);

        if (chunkRoot == nullptr)
        {
            return nullptr;
        }

        OctreeNode *chunkWithLod = getChunkWithLod(chunkRoot);

        generateMeshFromOctree(chunkWithLod, false, positions, normals, indices);

        vector<OctreeNode *> neighbouringChunks;

        vector<OctreeNode *> seamNodes = findSeamNodes(chunkWithLod, neighbouringChunks, chunksListHashMap, getChunkRootFromHashMap);

        OctreeNode *seamRoot = constructOctreeUpwards(seamRoot, seamNodes, chunkWithLod->min, chunkWithLod->size * 2);

        generateMeshFromOctree(seamRoot, true, positions, normals, indices);

        // deleting the chunk clone + neighbouring chunk clones
        for (int i = 0; i < neighbouringChunks.size(); i++)
        {
            destroyableNodesList.push_back(neighbouringChunks[i]);
        }
        // delete the chunk clone octree
        destroyableNodesList.push_back(chunkWithLod);
        // delete the seam octree
        destroyableNodesList.push_back(seamRoot);

        return getOutputBuffer(positions, normals, indices);
    }
}

// int main()
// {
//     const ivec3 OFFSETS[8] =
//         {
//             ivec3(0, 0, 0), ivec3(1, 0, 0), ivec3(0, 0, 1), ivec3(1, 0, 1),
//             ivec3(0, 1, 0), ivec3(1, 1, 0), ivec3(0, 1, 1), ivec3(1, 1, 1)};

//     DualContouring::generateChunkData(0, 0, 0);
//     DualContouring::generateChunkData(0, 0, -64);
//     // DualContouring::generateChunkData(64, 0, 0);
//     // DualContouring::generateChunkData(-64, 0, 0);
//     // DualContouring::generateChunkData(64, 0, -64);

//     DualContouring::setChunkLod(0, 0, 0, 1);
//     DualContouring::setChunkLod(0, 0, -64, 1);
//     // DualContouring::setChunkLod(64, 0, 0, 1);
//     // DualContouring::setChunkLod(-64, 0, 0, 1);
//     // DualContouring::setChunkLod(64, 0, -64, 1);

//     cout << "GENERATING IS DONE\n"
//          << endl;

//     cout << "\n"
//          << DualContouring::generateChunkMesh(0, 0, -64) << "\n\n"
//          << endl;

//     cout << "CHUNK IS CREATED\n\n"
//          << endl;

//     DualContouring::destroyUselessOctrees();

//     return 0;
// }