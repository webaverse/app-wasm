#include "occlusionCull.h"

PeekFace peekFaceSpecs[] = {
    {(int)PEEK_FACES::BACK, (int)PEEK_FACES::FRONT, 0, 0, -1},
    {(int)PEEK_FACES::FRONT, (int)PEEK_FACES::BACK, 0, 0, 1},
    {(int)PEEK_FACES::LEFT, (int)PEEK_FACES::RIGHT, -1, 0, 0},
    {(int)PEEK_FACES::RIGHT, (int)PEEK_FACES::LEFT, 1, 0, 0},
    {(int)PEEK_FACES::TOP, (int)PEEK_FACES::BOTTOM, 0, 1, 0},
    {(int)PEEK_FACES::BOTTOM, (int)PEEK_FACES::TOP, 0, -1, 0},
};

uint64_t hashMin(const ivec3 &min)
{
    uint64_t result = uint16_t(min.x);
    result = (result << 16) | uint16_t(min.y);
    result = (result << 16) | uint16_t(min.z);
    return result;
}

OcclusionCulling::OcclusionCulling()
{
    // initializing peek face indices
    for (int i = 0; i < 8 * 8; i++)
    {
        PEEK_FACE_INDICES[i] = 0xFF;
    }

    int peekIndex = 0;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (i != j)
            {
                int otherEntry = PEEK_FACE_INDICES[j << 3 | i];
                PEEK_FACE_INDICES[i << 3 | j] = otherEntry != 0xFF ? otherEntry : peekIndex++;
            }
        }
    }
}

uint8_t *OcclusionCulling::cull(uint8_t *chunksBuffer, const int &id, const ivec3 &min, const ivec3 &max, const vec3 &cameraPos, const int &numDraws)
{
    std::unordered_map<uint64_t, CullQueueEntry> cullableChunks;
    cullableChunks.reserve(numDraws);
    // std::cout << chunksBuffer[100] << std::endl;

    parseChunksBuffer(cullableChunks, chunksBuffer, numDraws);

    std::vector<int> culledList;

    const int range = max.x - min.x;
    std::queue<CullQueueEntry> cullQueue;

    CullQueueEntry firstEntry{id, ivec3{min.x, min.y - 64, min.z}, (int)PEEK_FACES::NONE, 0};
    cullQueue.push(firstEntry);

    while (cullQueue.size() > 0)
    {
        const CullQueueEntry entry = cullQueue.front();
        cullQueue.pop();

        const int x = entry.min.x;
        const int y = entry.min.y;
        const int z = entry.min.z;
        const int enterFace = entry.enterFace;

        for (int i = 0; i < 6; i++)
        {
            const PeekFace &peekFaceSpec = peekFaceSpecs[i];
            const int ay = y + peekFaceSpec.y * range;
            if (ay >= -320 && ay < -64)
            {
                const int ax = x + peekFaceSpec.x * range;
                const int az = z + peekFaceSpec.z * range;

                const ivec3 nextEntryMin = ivec3{ax, ay, az};
                const uint64_t hashedMin = hashMin(nextEntryMin);
                auto allocatedIter = cullableChunks.find(hashedMin);

                if (allocatedIter != cullableChunks.end())
                {
                    const CullQueueEntry &allocatedEntry = allocatedIter->second;
                    auto isCulledIter = std::find(culledList.begin(), culledList.end(), allocatedEntry.id);
                    if (isCulledIter == culledList.end())
                    {
                        culledList.push_back(allocatedEntry.id);
                        const CullQueueEntry newEntry{allocatedEntry.id, allocatedEntry.min, enterFace, allocatedEntry.peeks};
                        if (enterFace == (int)PEEK_FACES::NONE || entry.peeks[PEEK_FACE_INDICES[enterFace << 3 | peekFaceSpec.exitFace]] == 1)
                        {
                            cullQueue.push(newEntry);
                        }
                    }
                }
            }
        }
    }

    std::vector<int> drawList = getDrawList(cullableChunks, culledList, numDraws);

    return serializeDraws(drawList);
}

std::vector<int> OcclusionCulling::getDrawList(std::unordered_map<uint64_t, CullQueueEntry> &cullableCHunks, std::vector<int> &culledList, const int &numChunks)
{
    std::vector<int> drawList;
    drawList.resize(numChunks);

    for (const auto &p : cullableCHunks)
    {
        const int id = p.second.id;
        auto culledIter = std::find(culledList.begin(), culledList.end(), id);
        if (culledIter == culledList.end())
        {
            drawList.emplace_back(id);
        // std::cout << id << std::endl;
        }
    }
    // std::cout << "My name is jeff" << std::endl;

    return drawList;
}

void OcclusionCulling::parseChunksBuffer(std::unordered_map<uint64_t, CullQueueEntry> &cullableChunks, uint8_t *buffer, const int &numDraws)
{
    for (int i = 0; i < numDraws; i++)
    {
        int index = i * chunkDataSize;

        const int id = *((int *)(buffer + index));
        index += sizeof(int);

        const int minX = *((int *)(buffer + index));
        index += sizeof(int);

        const int minY = *((int *)(buffer + index));
        index += sizeof(int);

        const int minZ = *((int *)(buffer + index));
        index += sizeof(int);

        const int enterFace = *((int *)(buffer + index));
        index += sizeof(int);

        uint8_t peeks[numPeeksPerChunk];
        for (int j = 0; j < numPeeksPerChunk; j++)
        {
            peeks[j] = *((uint8_t *)(buffer + index));
            index += sizeof(uint8_t);
        }

        const ivec3 min = ivec3{minX, minY, minZ};
        const uint64_t hashedMin = hashMin(min);

        const CullQueueEntry entry = CullQueueEntry{id, min, enterFace, peeks};
        const std::pair<uint64_t, CullQueueEntry> chunkPair(hashedMin, entry);

        // std::cout << id << std::endl;

        cullableChunks.insert(chunkPair);
    }
};

uint8_t *OcclusionCulling::serializeDraws(const std::vector<int> &drawList)
{
    size_t neededSize = sizeof(uint32_t) + drawList.size() * sizeof(drawList[0]);
    uint8_t *buffer = (uint8_t *)malloc(neededSize);
    int index = 0;

    // culled list
    *((uint32_t *)(buffer + index)) = drawList.size();
    index += sizeof(uint32_t);
    std::memcpy(buffer + index, &drawList[0], drawList.size() * sizeof(drawList[0]));
    index += drawList.size() * sizeof(drawList[0]);

    return buffer;
};

OcclusionCulling *Culling::init()
{
    OcclusionCulling *instance = new OcclusionCulling();
    // std::cout << instance << std::endl;
    return instance;
};

uint8_t *Culling::cull(OcclusionCulling *inst,
                       uint8_t *chunksBuffer,
                       const int &id,
                       const ivec3 &min,
                       const ivec3 &max,
                       const vec3 &cameraPos,
                       const int &numDraws)
{
    return inst->cull(chunksBuffer, id, min, max, cameraPos, numDraws);
}
