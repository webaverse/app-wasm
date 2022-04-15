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
    const int NUM_BUFFER = 2;
    int *getMeshData(vector<Vector3> *vertices)
    {
        int *output = (int *)malloc(NUM_BUFFER * sizeof(int));
        output[0] = vertices->size();
        output[1] = (intptr_t) & (vertices->front());

        return output;
    }

    float getNoise(float x, float y, float z)
    {
        FastNoise fastNoise;
        return fastNoise.GetSimplexFractal(x, y, z);
        // return (1.0 + fastNoise.GetSimplexFractal(x, y, z)) / 2.0;
    }

    int *generateVertices(int segment)
    {
        vector<Vector3> *vertices = new vector<Vector3>;
        for (int x = 0; x < segment + 3; x++)
        {
            for (int y = 0; y < segment + 3; y++)
            {
                for (int z = 0; z < segment + 3; z++)
                {
                    Vector3 point;
                    point.x = x;
                    point.y = y;
                    point.z = z;
                    vertices->push_back(point);
                }
            }
        }
        return getMeshData(vertices);
    }

}

// int main()
// {

//     cout << "\n"
//          << DualContouring::generateVertices(10) << "\n"
//          << endl;
//     return 0;
// }