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
    uint8_t *constructOutputBuffer(VertexBuffer &vertexBuffer)
    {
        return vertexBuffer.getBuffer();
    }

    // chunk settings
    const int chunkSize = 64;

    // storing the octrees that we would delete after mesh construction
    std::vector<OctreeNode *> temporaryNodesList;

    // storing the octree roots here for search
    std::unordered_map<uint64_t, OctreeNode *> chunksListHashMap;

    void generateChunkData(float x, float y, float z)
    {
        const vm::ivec3 octreeMin = vm::ivec3(-chunkSize / 2) + vm::ivec3(x, y, z);
        CachedNoise chunkNoise(octreeMin, chunkSize);
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

//     // std::vector<std::pair<unsigned char, float>> sortedSamples = {
//     //     std::make_pair(2, 2.f),
//     //     std::make_pair(3, 3.f),
//     //     std::make_pair(2, 2.f),
//     //     std::make_pair(2, 2.f),
//     //     std::make_pair(4, 4.f),
//     //     std::make_pair(2, 2.f),
//     // };

//     // std::unordered_map<unsigned char, size_t> count;

//     // for (std::pair<unsigned char, float> s : sortedSamples)
//     // {
//     //     count[s.first]++;
//     // }

//     // std::sort(
//     //     sortedSamples.begin(),
//     //     sortedSamples.end(),
//     //     [&count](const std::pair<unsigned char, float> &a, const std::pair<unsigned char, float> &b)
//     //     {
//     //         if (a.first == b.first)
//     //         {
//     //             return false;
//     //         }
//     //         if (count[a.first] > count[b.first])
//     //         {
//     //             return true;
//     //         }
//     //         else if (count[a.first] < count[b.first])
//     //         {
//     //             return false;
//     //         }
//     //         return a.first < b.first;
//     //     });

//     // sortedSamples.erase(std::unique(sortedSamples.begin(), sortedSamples.end()), sortedSamples.end());

//     // int selectorCounter = 0;
//     // for (size_t i = 0; i < sortedSamples.size(); i++)
//     // {
//     //     std::cout << +sortedSamples.at(i).first << std::endl;
//     //     selectorCounter++;
//     // }
//     // for (size_t i = 0; i < 4 - selectorCounter; i++)
//     // {
//     //     std::cout << +sortedSamples.at(i).first << std::endl;
//     // }
    
    

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
//                 DualContouring::setChunkLod(x + 32, y + 32, z + 32, 4);
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
