#ifndef OCCLUSION_CULL_H
#define OCCLUSION_CULL_H

#include "iostream"
#include "queue"
#include "unordered_set"
#include "unordered_map"

struct vec3
{
    float x;
    float y;
    float z;
};

struct ivec3
{
    int x;
    int y;
    int z;
};

bool isVectorInRange(const vec3 &vector, const ivec3 &min, const ivec3 &max)
{
    return (vector.x >= min.x && vector.x < max.x) && (vector.y >= min.y && vector.y < max.y) && (vector.z >= min.z && vector.z < max.z);
}

enum class PEEK_FACES : int
{
    FRONT = 0,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    NONE
};

struct PeekFace
{
    int exitFace;
    int enterFace;
    int x;
    int y;
    int z;
};

PeekFace peekFaceSpecs[] = {
    {(int)PEEK_FACES::BACK, (int)PEEK_FACES::FRONT, 0, 0, -1},
    {(int)PEEK_FACES::FRONT, (int)PEEK_FACES::BACK, 0, 0, 1},
    {(int)PEEK_FACES::LEFT, (int)PEEK_FACES::RIGHT, -1, 0, 0},
    {(int)PEEK_FACES::RIGHT, (int)PEEK_FACES::LEFT, 1, 0, 0},
    {(int)PEEK_FACES::TOP, (int)PEEK_FACES::BOTTOM, 0, 1, 0},
    {(int)PEEK_FACES::BOTTOM, (int)PEEK_FACES::TOP, 0, -1, 0},
};

struct CullQueueEntry
{
    bool operator==(CullQueueEntry const &other) const
    {
        return (id == other.id &&
                min.x == other.min.x &&
                min.y == other.min.y &&
                min.z == other.min.z &&
                enterFace == other.enterFace &&
                peeks == other.peeks);
    }
    int id;
    ivec3 min;
    int enterFace;
    uint8_t *peeks;
};

uint64_t hashMin(const ivec3 &min)
{
    uint64_t result = uint16_t(min.x);
    result = (result << 16) | uint16_t(min.y);
    result = (result << 16) | uint16_t(min.z);
    return result;
}

class OcclusionCulling
{
public:
    std::unordered_map<uint64_t, CullQueueEntry> allocatedMap;
    std::unordered_map<uint64_t, CullQueueEntry> culledSet;
    int PEEK_FACE_INDICES[8 * 8];

    OcclusionCulling()
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
        // reserving space for culledSet
        // culledSet.reserve(256);
        // std::cout << "Occlusion Culling init" << std::endl;
    }

    void alloc(const int &id,
               const ivec3 &min,
               uint8_t *peeks)
    {
        const uint64_t hashedMin = hashMin(min);
        std::pair<uint64_t, CullQueueEntry> cullPair(hashedMin, CullQueueEntry{id, min, (int)PEEK_FACES::NONE, peeks});
        allocatedMap.insert(cullPair);
        // std::cout << allocatedMap[0].min.x << std::endl;
    }

    void free(const ivec3 &min)
    {
        const uint64_t hashedMin = hashMin(min);
        allocatedMap.erase(hashedMin);
    }

    void cull(const int &id,
              const vec3 &cameraPos,
              const ivec3 &min,
              const ivec3 &max)
    {
        const int range = max.x - min.x;

        if (isVectorInRange(cameraPos, min, max))
        {
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
                        auto allocatedIter = allocatedMap.find(hashedMin);

                        if (allocatedIter != allocatedMap.end())
                        {
                            const CullQueueEntry &allocatedEntry = allocatedIter->second;
                            auto culledIter = culledSet.find(hashedMin);
                            if (culledIter == culledSet.end())
                            {
                                std::pair<uint64_t, CullQueueEntry> culledPair(hashedMin, allocatedEntry);
                                culledSet.insert(culledPair);
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
        }
    }
};

namespace Culling
{

    OcclusionCulling *createOcclusionCullingInstance()
    {
        OcclusionCulling *instance = new OcclusionCulling();
        return instance;
    }

    void allocateOcclusionCulling(OcclusionCulling *inst, const int &id,
                                  const int &minX, const int &minY, const int &minZ,
                                  uint8_t *peeks)
    {
        // std::cout << inst->PEEK_FACE_INDICES[12] << std::endl;
        inst->alloc(id, ivec3{minX, minY, minZ}, peeks);
    }

    void freeOcclusionCulling(OcclusionCulling *inst, const int &minX, const int &minY, const int &minZ)
    {
        // inst->free(ivec3{minX, minY, minZ});
    }

    void setVisibilityOcclusionCulling(OcclusionCulling *inst,
                                       const int &id,
                                       const float &cameraX, const float &cameraY, const float &cameraZ,
                                       int &minX, const int &minY, const int &minZ,
                                       const int &maxX, const int &maxY, const int &maxZ)
    {
        // inst->cull(id, vec3{cameraX, cameraY, cameraZ}, ivec3{minX, minY, minZ}, ivec3{maxX, maxY, maxZ});
    }

    bool getDrawVisibilityOcclusionCulling(OcclusionCulling *inst, const int &minX, const int &minY, const int &minZ)
    {
        const ivec3 min = ivec3{minX, minY, minZ};
        const uint64_t hashedMin = hashMin(min);
        auto culledIter = inst->culledSet.find(hashedMin);
        if (culledIter == inst->culledSet.end())
        {
            return false;
        }
        else
        {
            return false;
        }
    }

}
#endif // OCCLUSION_CULL_H