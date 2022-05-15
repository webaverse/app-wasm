#include "main.h"
#include "octree.h"

class BufferManager
{
private:
    // we store the pointers to each data that we want in outputBuffer
    std::vector<int> outputBuffer;
    template <typename T>
    int *copyVectorToMemory(const std::vector<T> vector)
    {
        unsigned int size = vector.size() * sizeof(vector[0]);
        int *buffer = (int *)malloc(size);
        memcpy(buffer, &vector.front(), size);
        return buffer;
    }

public:
    template <typename T>
    void appendVector(const std::vector<T> vector)
    {
        outputBuffer.push_back((intptr_t)copyVectorToMemory(vector));
    }

    template <typename T>
    void appendVectorSize(const std::vector<T> vector)
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
    int *getOutputBuffer(VertexBuffer &vertexBuffer)
    {

        // the order of appending data to the buffer is important,
        // we will read the data by the same order in javascript
        BufferManager bufferController;

        bufferController.appendVectorSize(vertexBuffer.positions);
        bufferController.appendVector(vertexBuffer.positions);

        bufferController.appendVectorSize(vertexBuffer.normals);
        bufferController.appendVector(vertexBuffer.normals);

        bufferController.appendVectorSize(vertexBuffer.indices);
        bufferController.appendVector(vertexBuffer.indices);

        bufferController.appendVectorSize(vertexBuffer.biomes);
        bufferController.appendVector(vertexBuffer.biomes);

        bufferController.appendVectorSize(vertexBuffer.biomesWeights);
        bufferController.appendVector(vertexBuffer.biomesWeights);

        return bufferController.getOutputBuffer();
    }

    // chunk settings
    int chunkSize = 64;

    // storing the octrees that we would delete after mesh construction
    std::vector<OctreeNode *> destroyableNodesList;

    // storing the octree roots here for search
    std::unordered_map<uint64_t, OctreeNode *> chunksListHashMap;

    void generateChunkData(float x, float y, float z)
    {
        const glm::ivec3 octreeMin = glm::ivec3(-chunkSize / 2) + glm::ivec3(x, y, z);
        OctreeNode *chunk = constructOctreeDownwards(octreeMin, chunkSize);
        if (!chunk)
        {
            return;
        }
        addChunkRootToHashMap(chunk, chunksListHashMap);
    }

    void setChunkLod(float x, float y, float z, const float lod)
    {
        const glm::ivec3 octreeMin = glm::ivec3(-chunkSize / 2) + glm::ivec3(x, y, z);
        OctreeNode *chunkRoot = getChunkRootFromHashMap(octreeMin, chunksListHashMap);
        if (chunkRoot == nullptr)
        {
            // std::cout << "NULL :/ for no reason" << std::endl;
            return;
        }
        chunkRoot->lod = LodLevel(lod);
    }

    void destroyUselessOctrees()
    {
        std::vector<OctreeNode *> nodesList;
        // nodesList.reserve(50000);
        for (int i = 0; i < destroyableNodesList.size(); i++)
        {
            addNodesToVector(destroyableNodesList[i], nodesList);
        }
        // sort and remove duplicates
        std::sort(nodesList.begin(), nodesList.end());
        nodesList.erase(std::unique(nodesList.begin(), nodesList.end()), nodesList.end());
        // std::cout << nodesList.size() << std::endl;
        for (int i = 0; i < nodesList.size(); i++)
        {
            const OctreeNode *node = nodesList[i];
            if (node->drawInfo)
            {
                delete node->drawInfo;
            }
            delete node;
        }
        destroyableNodesList.clear();
    }

    int *generateChunkMesh(float x, float y, float z)
    {
        VertexBuffer vertexBuffer;

        const glm::ivec3 octreeMin = glm::ivec3(-chunkSize / 2) + glm::ivec3(x, y, z);

        OctreeNode *chunkRoot = getChunkRootFromHashMap(octreeMin, chunksListHashMap);
        if (!chunkRoot)
        {
            return nullptr;
        }
        OctreeNode *chunkWithLod = createChunkWithLod(chunkRoot);
        generateMeshFromOctree(chunkWithLod, false, vertexBuffer);

        std::vector<OctreeNode *> neighbouringChunks;
        std::vector<OctreeNode *> seamNodes = findSeamNodes(chunkWithLod, neighbouringChunks, chunksListHashMap, getChunkRootFromHashMap);
        OctreeNode *seamRoot = constructOctreeUpwards(seamRoot, seamNodes, chunkWithLod->min, chunkWithLod->size * 2);
        generateMeshFromOctree(seamRoot, true, vertexBuffer);

        // deleting the chunk clone + neighbouring chunk clones
        for (int i = 0; i < neighbouringChunks.size(); i++)
        {
            destroyableNodesList.push_back(neighbouringChunks[i]);
        }
        // delete the chunk clone octree
        destroyableNodesList.push_back(chunkWithLod);
        // delete the seam octree
        destroyableNodesList.push_back(seamRoot);

        return getOutputBuffer(vertexBuffer);
    }
}

// int main()
// {
//     const glm::ivec3 OFFSETS[8] =
//         {
//             glm::ivec3(0, 0, 0), glm::ivec3(1, 0, 0), glm::ivec3(0, 0, 1), glm::ivec3(1, 0, 1),
//             glm::ivec3(0, 1, 0), glm::ivec3(1, 1, 0), glm::ivec3(0, 1, 1), glm::ivec3(1, 1, 1)};

//     const int min = -((64 * 1) / 2);
//     const int max = (64 * 1) / 2;

//     for (int x = min; x < max; x += 64)
//     {
//         for (int y = min; y < max; y += 64)
//         {
//             for (int z = min; z < max; z += 64)
//             {
//                 std::cout << "Generating" << std::endl;
//                 DualContouring::generateChunkData(x + 32, y + 32, z + 32);
//             }
//         }
//     }

//     for (int x = min; x < max; x += 64)
//     {
//         for (int y = min; y < max; y += 64)
//         {
//             for (int z = min; z < max; z += 64)
//             {
//                 const int lod = abs(glm::max(glm::max(x, y), z)) / 64;
//                 DualContouring::setChunkLod(x + 32, y + 32, z + 32, lod);
//             }
//         }
//     }

//     for (int x = min; x < max; x += 64)
//     {
//         for (int y = min; y < max; y += 64)
//         {
//             for (int z = min; z < max; z += 64)
//             {
//                 std::cout << "\n"
//                           << DualContouring::generateChunkMesh(x + 32, y + 32, z + 32)
//                           << std::endl;
//                 DualContouring::destroyUselessOctrees();
//             }
//         }
//     }

//     // std::cout << "GENERATING IS DONE\n"
//     //      << std::endl;

//     // std::cout << "\n"
//     //      << DualContouring::generateChunkMesh(0, 0, -64) << "\n\n"
//     //      << std::endl;

//     // std::cout << "CHUNK IS CREATED\n\n"
//     //      << std::endl;

//     // DualContouring::destroyUselessOctrees();

//     return 0;
// }