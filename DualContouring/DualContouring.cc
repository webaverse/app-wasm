#include "dualContouring.h"

namespace DualContouring
{
    float terrainSurface = 0.5;

    float simplexNoise(float x, float y, float z)
    {
        FastNoise fastNoise;
        return fastNoise.GetSimplexFractal(x, y, z);
        // return (1.0 + fastNoise.GetSimplexFractal(x, y, z)) / 2.0;
    }

    void populateTerrainMap(int width, int height, int depth)
    {
        for (int x = 0; x < width + 1; x++)
        {
            for (int y = 0; y < height + 1; y++)
            {
                for (int z = 0; z < depth + 1; z++)
                {
                    float thisHeight = (float)height * simplexNoise((float)x / 16.0 * 1.5 + 0.001, (float)y / 16.0 * 1.5 + 0.001, (float)z / 16.0 * 1.5 + 0.001);
                }
            }
        }
    }

    vector<vec3> generateVertices(int width, int height, int depth)
    {
        vector<vec3> vertices;
        for (int x = 0; x < width + 3; x++)
        {
            for (int y = 0; y < height + 3; y++)
            {
                for (int z = 0; z < depth + 3; z++)
                {
                    vec3 point;
                    point.x = x;
                    point.y = y;
                    point.z = z;
                    vertices.push_back(point);
                }
            }
        }
        return vertices;
    }
}

