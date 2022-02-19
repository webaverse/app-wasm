namespace Terrain {

	struct Vector3 {
		float x; float y; float z;
	};

	struct Vector4 {
		float x; float y; float z; float w;
	};

	float* generateTerrain(float size, int levelCount, int maxSegment);

	void createChunk(
		float origin[3], float chunkSize, int segment,
    	float *vertices, uint32_t *indices, int vertexOffset, int indexOffset, int & indexCount
    );
}
