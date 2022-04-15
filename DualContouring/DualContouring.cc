// #include <math.h>
// #include <map>
// #include <algorithm>
#include <cstdint>
#include <ctime>
#include <vector>
#include <iostream>
#include <string.h>
#include <iostream>
#include <array>
#include "DualContouring.h"

using namespace std;

namespace DualContouring
{
    float terrainSurface = 0.5;
    const int NUM_BUFFER = 2;
    int *getMeshData(vector<Vector3> vertices)
    {
        unsigned int positionsSize = vertices.size() * sizeof(vertices[0]);
        int *positions = (int *)malloc(positionsSize);
        memcpy(positions, &vertices.front(), positionsSize);

        int *output = (int *)malloc(NUM_BUFFER * sizeof(int));
        output[0] = vertices.size();
        output[1] = (intptr_t)positions;
        
        return output;
    }

    float getNoise(float x, float y, float z)
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
                    float thisHeight = (float)height * getNoise((float)x / 16.0 * 1.5 + 0.001, (float)y / 16.0 * 1.5 + 0.001, (float)z / 16.0 * 1.5 + 0.001);
                }
            }
        }
    }

    int *generateVertices(int width, int height, int depth)
    {
        vector<Vector3> vertices;
        for (int x = 0; x < width + 3; x++)
        {
            for (int y = 0; y < height + 3; y++)
            {
                for (int z = 0; z < depth + 3; z++)
                {
                    Vector3 point;
                    point.x = x;
                    point.y = y;
                    point.z = z;
                    vertices.push_back(point);
                }
            }
        }
        return getMeshData(vertices);
    }
}

int main()
{
    cout << "\n"
         << DualContouring::generateVertices(10,10,10) << "\n"
         << endl;
    return 0;
}