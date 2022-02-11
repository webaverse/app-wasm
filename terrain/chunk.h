#include <vector>
#include <map>
#include <string>
#include "vector.h"

namespace Terrain
{
	struct Vector3 {
		float x; float y; float z;
	};

	struct Vector4 {
		float x; float y; float z; float w;
	};

	class Chunk
	{
		public:
			Chunk(float origin[3], float chunkSize, int level, float x, float z);
			~Chunk();
			float* getGeometryBuffer();
		private:
			void build();
			void density(int x, int y, int z);
			void march(int x, int y, int z);
			int indexFromCoord(int x, int y, int z);
			Vector3 interpolateVerts(Vector4 v1, Vector4 v2);

			std::vector<Vector3> vertices;
			std::vector<int> indices;
			std::vector<Vector4> points;
			std::map<std::string, int> vertexDic;

			int segment = 32;
			float unitSize;
			Vector3 origin;
			Vector4 min;
			Vector4 max;
			int index;
	};
}
