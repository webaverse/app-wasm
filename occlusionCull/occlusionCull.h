#ifndef OCCLUSION_CULL_H
#define OCCLUSION_CULL_H

#include "iostream"
#include "queue"
#include "unordered_set"
#include "unordered_map"
#include "../vector.h"
#include <emscripten.h>

const int defaultChunkSize = 16;

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
    ivec3 offset;
};

struct CullQueueEntry
{
    int id;         // 4 bytes
    ivec3 min;      // 12 bytes
    int enterFace;  // 4 bytes
    uint8_t *peeks; // 15 bytes
    int lod;        // 4 bytes
};

const int numPeeksPerChunk = 15;
const int chunkDataSize = sizeof(int) + 3 * sizeof(int) + sizeof(int) + numPeeksPerChunk * sizeof(uint8_t) + sizeof(int); // 4 + 12 + 4 + 15

const int airChunkId = -1;

typedef std::unordered_map<uint64_t, CullQueueEntry> CullQueueEntryMap;
class OcclusionCulling
{
public:
    int PEEK_FACE_INDICES[8 * 8];

    OcclusionCulling();

    uint8_t *cull(uint8_t *chunksBuffer,
                  uint8_t *chunkNodesMapBuffer,
                  const int &id,
                  const ivec3 &min,
                  const ivec3 &max,
                  const vec3 &cameraPos,
                  const vec3 &cameraView,
                  const int &lod,
                  const int &numDraws,
                  const int &numNodes);

private:
    std::vector<int> sortDraws(CullQueueEntryMap &cullableCHunks,
                               std::vector<int> &culledList,
                               const vec3 &cameraPos,
                               const int &numChunks);

    void deserializeChunkNodesMap(std::vector<uint64_t> &chunkNodes, uint8_t *buffer, const int &numNodes);

    void deserializeCullableChunks(CullQueueEntryMap &cullableChunks,
                                   uint8_t *buffer,
                                   const int &numDraws);

    uint8_t *serializeDraws(const std::vector<int> &drawList);
};

namespace Culling
{
    OcclusionCulling *init();
    uint8_t *cull(OcclusionCulling *inst,
                  uint8_t *chunksBuffer,
                  uint8_t *chunkNodesMapBuffer,
                  const int &id,
                  const ivec3 &min,
                  const ivec3 &max,
                  const vec3 &cameraPos,
                  const vec3 &cameraView,
                  const int &lod,
                  const int &numDraws,
                  const int &numNodes);
};

#endif // OCCLUSION_CULL_H