#include <vector>

namespace Terrain {

	struct Vector3 {
		float x; float y; float z;
	};

	struct Vector4 {
		float x; float y; float z; float w;
	};

	struct Range {
        int offset;
        int size;
    };

    struct VertexBiome {
	   float data[8];
    };

	float* generateTerrain(
		float size, int levelCount, int maxSegment, int vertexBufferSizeParam, int indexBufferSizeParam
	);

	void deallocateChunk(
	  int vetexSlot, int indexSlot, int totalChunkCount,
	  int *chunkVertexRangeBuffer,
	  int *vertexFreeRangeBuffer,
	  int *chunkIndexRangeBuffer,
	  int *indexFreeRangeBuffer
	);

	int* generateAndAllocateChunk(
	  float *vertexBuffer, float *normalBuffer, float *biomeBuffer, int *indexBuffer,
	  int *chunkVertexRangeBuffer,
	  int *vertexFreeRangeBuffer,
	  int *chunkIndexRangeBuffer,
	  int *indexFreeRangeBuffer,
	  float x, float y, float z, float chunkSize, int segment, int totalChunkCount
	);

	void createChunk(
		float origin[3], float chunkSize, int segment,
    	std::vector<Vector3> & vertices, std::vector<Vector3> & normals, std::vector<VertexBiome> & biomes, std::vector<int> & indices
    );
}
