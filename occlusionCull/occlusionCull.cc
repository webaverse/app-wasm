#include "occlusionCull.h"

PeekFace peekFaceSpecs[] = {
    {(int)PEEK_FACES::BACK, (int)PEEK_FACES::FRONT, ivec3{0, 0, -1}},
    {(int)PEEK_FACES::FRONT, (int)PEEK_FACES::BACK, ivec3{0, 0, 1}},
    {(int)PEEK_FACES::LEFT, (int)PEEK_FACES::RIGHT, ivec3{-1, 0, 0}},
    {(int)PEEK_FACES::RIGHT, (int)PEEK_FACES::LEFT, ivec3{1, 0, 0}},
    {(int)PEEK_FACES::TOP, (int)PEEK_FACES::BOTTOM, ivec3{0, 1, 0}},
    {(int)PEEK_FACES::BOTTOM, (int)PEEK_FACES::TOP, ivec3{0, -1, 0}},
};

uint8_t airChunkPeeks[15] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

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

uint8_t *OcclusionCulling::cull(uint8_t *chunksBuffer, const int &id, const ivec3 &min, const ivec3 &max, const vec3 &cameraPos, const vec3 &cameraView, const int &lod, const int &numDraws)
{
    std::unordered_map<uint64_t, CullQueueEntry> cullableChunks;
    deserializeCullableChunks(cullableChunks, chunksBuffer, numDraws);

    std::vector<int> culledList;

    const int range = max.x - min.x;
    std::queue<CullQueueEntry> cullQueue;

    CullQueueEntry firstEntry{id, ivec3{min.x, min.y + 70, min.z}, (int)PEEK_FACES::NONE, 0, lod};
    cullQueue.push(firstEntry);
    // std::cout << "SY :" << min.y << std:: endl;

    std::unordered_map<uint64_t, CullQueueEntry> seenChunks;

    while (cullQueue.size() > 0)
    {
        const CullQueueEntry entry = cullQueue.front();
        cullQueue.pop();

        const int x = entry.min.x;
        const int y = entry.min.y;
        const int z = entry.min.z;
        const int enterFace = entry.enterFace;
        const int lod = entry.lod;

        // if(lod == 4){
        //     std::cout << enterFace << std::endl;
        // }

        for (int i = 0; i < 6; i++)
        {
            const PeekFace &peekFaceSpec = peekFaceSpecs[i];
            const int ay = y + peekFaceSpec.offset.y * 16 * lod;
            const int ax = x + peekFaceSpec.offset.x * 16 * lod;
            const int az = z + peekFaceSpec.offset.z * 16 * lod;
            if (ax >= firstEntry.min.x - 15 * 16 && ax <= firstEntry.min.x + 15 * 16)
            if (ay >= firstEntry.min.y - 15 * 16 && ay <= firstEntry.min.y + 15 * 16)
            if (az >= firstEntry.min.z - 15 * 16 && az <= firstEntry.min.z + 15 * 16)
            {
                CullQueueEntry newEntry;

                const ivec3 nextEntryMin = ivec3{ax, ay, az};
                const uint64_t hashedMin = hashMin(nextEntryMin);

                auto seenIter = seenChunks.find(hashedMin);
                if (seenIter == seenChunks.end())
                {
                    const std::pair<uint64_t, CullQueueEntry> seenPair(hashedMin, newEntry);
                    seenChunks.insert(seenPair);

                    auto allocatedIter = cullableChunks.find(hashedMin);
                    if (allocatedIter != cullableChunks.end())
                    {
                        const CullQueueEntry &allocatedEntry = allocatedIter->second;

                        auto isCulledIter = std::find(culledList.begin(), culledList.end(), allocatedEntry.id);
                        if (isCulledIter == culledList.end())
                        {
                            culledList.push_back(allocatedEntry.id);
                        }

                        newEntry = {allocatedEntry.id, nextEntryMin, peekFaceSpec.enterFace, allocatedEntry.peeks, allocatedEntry.lod};
                    }
                    else
                    {
                        newEntry = {airChunkId, nextEntryMin, peekFaceSpec.enterFace, airChunkPeeks, lod};
                    }
                    if (enterFace == (int)PEEK_FACES::NONE || entry.peeks[PEEK_FACE_INDICES[enterFace << 3 | peekFaceSpec.exitFace]] == 1)
                    {
                        cullQueue.push(newEntry);
                    }
                }
            }
        }
    }

    std::vector<int> drawList = sortDraws(cullableChunks, culledList, cameraPos, numDraws);

    return serializeDraws(drawList);
}

float distanceToCamera(const ivec3 &min, const vec3 &cameraPos)
{
    const float ax = min.x - cameraPos.x;
    const float ay = min.y - cameraPos.y;
    const float az = min.z - cameraPos.z;
    return std::sqrt(ax * ax + ay * ay + az * az);
}

std::vector<int> OcclusionCulling::sortDraws(CullQueueEntryMap &cullableCHunks,
                                             std::vector<int> &culledList,
                                             const vec3 &cameraPos,
                                             const int &numChunks)
{
    std::vector<int> drawList;
    std::vector<std::pair<int, float>> sortedDraws;

    const int drawListSize = numChunks - culledList.size();
    sortedDraws.reserve(drawListSize);

    // ! culled draws percentage :
    std::cout << (float)culledList.size() / (float)numChunks * 100.f << "%" << std::endl;

    for (const auto &p : cullableCHunks)
    {
        const int id = p.second.id;
        const ivec3 min = p.second.min;
        const float distance = distanceToCamera(min, cameraPos);
        auto culledIter = std::find(culledList.begin(), culledList.end(), id);
        if (culledIter == culledList.end())
        {
            sortedDraws.emplace_back(id, distance);
        }
    }

    std::sort(sortedDraws.begin(),
              sortedDraws.end(),
              [&](const std::pair<int, float> &a, const std::pair<int, float> &b) -> bool
              { return a.second > b.second; });

    drawList.resize(drawListSize);

    for (int i = 0; i < drawListSize; i++)
    {
        drawList[i] = sortedDraws[i].first;
    }

    return drawList;
}

void OcclusionCulling::deserializeCullableChunks(CullQueueEntryMap &cullableChunks, uint8_t *buffer, const int &numDraws)
{
    // deserializing
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

        const int lod = *((int *)(buffer + index));
        index += sizeof(int);

        const ivec3 min = ivec3{minX, minY, minZ};
        const uint64_t hashedMin = hashMin(min);

        const CullQueueEntry entry = CullQueueEntry{id, min, enterFace, peeks, lod};
        const std::pair<uint64_t, CullQueueEntry> chunkPair(hashedMin, entry);

        cullableChunks.insert(chunkPair);
    }
};

uint8_t *OcclusionCulling::serializeDraws(const std::vector<int> &drawList)
{
    // serializing
    int drawListNumSize = sizeof(uint32_t);
    int drawListBytesNum = drawList.size() * sizeof(drawList[0]);

    size_t neededSize = drawListNumSize + drawListBytesNum;

    uint8_t *buffer = (uint8_t *)malloc(neededSize);
    int index = 0;

    // culled list
    *((uint32_t *)(buffer + index)) = drawList.size(); // Draws Count
    index += drawListNumSize;

    std::memcpy(buffer + index, &drawList[0], drawListBytesNum); // Draw List
    index += drawListBytesNum;

    return buffer;
};

OcclusionCulling *Culling::init()
{
    OcclusionCulling *instance = new OcclusionCulling();
    return instance;
};

uint8_t *Culling::cull(OcclusionCulling *inst,
                       uint8_t *chunksBuffer,
                       const int &id,
                       const ivec3 &min,
                       const ivec3 &max,
                       const vec3 &cameraPos,
                       const vec3 &cameraView,
                       const int &lod,
                       const int &numDraws)
{
    return inst->cull(chunksBuffer, id, min, max, cameraPos, cameraView, lod, numDraws);
}
