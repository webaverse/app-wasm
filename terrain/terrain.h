namespace Terrain {

	struct Vector3 {
		float x; float y; float z;
	};

	struct Vector4 {
		float x; float y; float z; float w;
	};

	float* generateTerrain(float size, int levelCount, int maxSegment, int vertexStrideParam, int faceStrideParam);

	void createChunk(
		float origin[3], float chunkSize, int segment,
    	float *vertices, float *normals, int *indices, int vertexOffset, int indexOffset, int & indexCount
    );
}
