#include "main.h"
#include "octree.h"
#include "bufferManager.h"

namespace DualContouring
{
    int *constructOutputBuffer(VertexBuffer &vertexBuffer)
    {
        // the order of appending data to the buffer is important,
        // we will read the data by the same order in javascript
        BufferManager bm;

        bm.appendVectorSize(vertexBuffer.positions);
        bm.appendVector(vertexBuffer.positions);

        bm.appendVectorSize(vertexBuffer.normals);
        bm.appendVector(vertexBuffer.normals);

        bm.appendVectorSize(vertexBuffer.indices);
        bm.appendVector(vertexBuffer.indices);

        bm.appendVectorSize(vertexBuffer.biomes);
        bm.appendVector(vertexBuffer.biomes);

        bm.appendVectorSize(vertexBuffer.biomesWeights);
        bm.appendVector(vertexBuffer.biomesWeights);

        return bm.getOutput();
    }

    // chunk settings
    int chunkSize = 64;

    // storing the octrees that we would delete after mesh construction
    std::vector<OctreeNode *> temporaryNodesList;

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

    void setChunkLod(float x, float y, float z, const int lod)
    {
        const glm::ivec3 octreeMin = glm::ivec3(-chunkSize / 2) + glm::ivec3(x, y, z);
        OctreeNode *chunkRoot = getChunkRootFromHashMap(octreeMin, chunksListHashMap);
        if (chunkRoot == nullptr)
        {
            // std::cout << "NULL :/ for no reason" << std::endl;
            return;
        }
        chunkRoot->lod = static_cast<LodLevel>(lod);
    }

    void clearTemporaryChunkData()
    {
        std::vector<OctreeNode *> nodesList;
        for (int i = 0; i < temporaryNodesList.size(); i++)
        {
            addNodesToVector(temporaryNodesList[i], nodesList);
        }
        // sort and remove duplicates
        std::sort(nodesList.begin(), nodesList.end());
        nodesList.erase(std::unique(nodesList.begin(), nodesList.end()), nodesList.end());
        for (int i = 0; i < nodesList.size(); i++)
        {
            const OctreeNode *node = nodesList[i];
            if (node->drawInfo)
            {
                delete node->drawInfo;
            }
            delete node;
        }
        temporaryNodesList.clear();
    }

    void clearChunkRoot(float x, float y, float z){
        // we destroy the chunk root separately because we might need it for LOD switch if it's already generated
        const glm::ivec3 octreeMin = glm::ivec3(-chunkSize / 2) + glm::ivec3(x, y, z);
        OctreeNode *chunkRoot = getChunkRootFromHashMap(octreeMin, chunksListHashMap);
        if (!chunkRoot)
        {
            return;
        }
        destroyOctree(chunkRoot);
    }

    int *createChunkMesh(float x, float y, float z)
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

        // adding the chunk clone + neighbouring chunk clones to the destroyable list
        for (int i = 0; i < neighbouringChunks.size(); i++)
        {
            temporaryNodesList.push_back(neighbouringChunks[i]);
        }
        // add the chunk clone octree to the destroyable list
        temporaryNodesList.push_back(chunkWithLod);
        // add the seam octree to the destroyable list
        temporaryNodesList.push_back(seamRoot);

        return constructOutputBuffer(vertexBuffer);
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
//                           << DualContouring::createChunkMesh(x + 32, y + 32, z + 32)
//                           << std::endl;
//                 DualContouring::clearTemporaryChunkData();
//                 DualContouring::clearChunkRoot(x + 32, y + 32, z + 32);
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

//     // DualContouring::deleteTemporaryChunkData();

//     return 0;
// }
