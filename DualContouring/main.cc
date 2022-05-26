#include "main.h"

// namespace ChunkMesh
// {
//     OctreeNode *chunkRoot;
//     OctreeNode *chunkWithLod;
//     std::vector<OctreeNode *> neighbouringChunks;
//     OctreeNode *seamRoot;
// };

namespace DualContouring
{
    int *constructOutputBuffer(VertexBuffer &vertexBuffer)
    {
        // the order of appending data to the buffer is important,
        // we will read the data by the same order in javascript
        BufferManager bm;

        bm.appendVectorSize(vertexBuffer.positions); // 0 positions vector size
        bm.appendVector(vertexBuffer.positions);     // 1 positions vector

        bm.appendVectorSize(vertexBuffer.normals); // 2 normals vector size
        bm.appendVector(vertexBuffer.normals);     // 3 normals vector

        bm.appendVectorSize(vertexBuffer.indices); // 4 indices vector size
        bm.appendVector(vertexBuffer.indices);     // 5 indices vector

        bm.appendVectorSize(vertexBuffer.biomes); // 6 biomes vector size
        bm.appendVector(vertexBuffer.biomes);     // 7 biomes vector

        bm.appendVectorSize(vertexBuffer.biomesWeights); // 8 biomesWeight vector size
        bm.appendVector(vertexBuffer.biomesWeights);     // 9 biomesWeight vector

        return bm.getOutput();
    }

    // chunk settings
    const int chunkSize = 64;
    const int randomSeed = 1000;

    // storing the octrees that we would delete after mesh construction
    std::vector<OctreeNode *> temporaryNodesList;

    // storing the octree roots here for search
    std::unordered_map<uint64_t, OctreeNode *> chunksListHashMap;

    void generateChunkData(float x, float y, float z)
    {
        const vm::ivec3 octreeMin = vm::ivec3(-chunkSize / 2) + vm::ivec3(x, y, z);
        CachedNoise chunkNoise(octreeMin, chunkSize);
        // std::cout << "NOISE IS GENERATED" << std::endl;
        OctreeNode *chunk = constructOctreeDownwards(octreeMin, chunkSize, chunkNoise);
        if (!chunk)
        {
            return;
        }
        addChunkRootToHashMap(chunk, chunksListHashMap);
    }

    void setChunkLod(float x, float y, float z, const int lod)
    {
        const vm::ivec3 octreeMin = vm::ivec3(-chunkSize / 2) + vm::ivec3(x, y, z);
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

    void clearChunkRoot(float x, float y, float z)
    {
        // we destroy the chunk root separately because we might need it for LOD switch if it's already generated
        const vm::ivec3 octreeMin = vm::ivec3(-chunkSize / 2) + vm::ivec3(x, y, z);
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

        const vm::ivec3 octreeMin = vm::ivec3(-chunkSize / 2) + vm::ivec3(x, y, z);
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
//     // CachedNoise sampleNoise;
//     // const double noise = sampleNoise.simplex(30,500);
//     // std::cout << noise << std::endl;

//     const vm::ivec3 OFFSETS[8] =
//         {
//             vm::ivec3(0, 0, 0), vm::ivec3(1, 0, 0), vm::ivec3(0, 0, 1), vm::ivec3(1, 0, 1),
//             vm::ivec3(0, 1, 0), vm::ivec3(1, 1, 0), vm::ivec3(0, 1, 1), vm::ivec3(1, 1, 1)};

//     const int min = -((64 * 2) / 2);
//     const int max = (64 * 2) / 2;

//     for (int x = min; x < max; x += 64)
//     {
//         for (int y = min; y < max; y += 64)
//         {
//             for (int z = min; z < max; z += 64)
//             {
//                 // std::cout << "Generating" << std::endl;
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
//                 const int lod = abs(std::max(std::max(x, y), z)) / 64;
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

//     return 0;
// }
