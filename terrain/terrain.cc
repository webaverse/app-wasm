// #include <math.h>
// #include <map>
// #include <algorithm>
// #include <cstring>
#include <ctime>
#include <vector>
#include <iostream>
#include "terrain.h"

using namespace std;

namespace Terrain
{
    int *generateRandomVertices(int vertexCount)
    {
        vector<Vector3> vertices{};
        for (int i = 0; i < vertexCount; i++)
        {

            srand(unsigned(time(0)));
            float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10.0);
            float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10.0);
            float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 10.0);
            Vector3 vertex = {x, y, z};
            vertices.push_back(vertex);
        }

        // int *vertexBuffer = (int *)malloc(vertexCount * 3 * sizeof(float));
        // memcpy(vertexBuffer + vertexCount * 3, &(vertices.front()), vertices.size() * sizeof(Vector3));

        // int *outputBuffer = (int *)malloc(4 * sizeof(int));
        // outputBuffer[0] = &vertices;

        // return outputBuffer;
    }
}

// int main()
// {

//     cout << "\n"
//          << Terrain::generateRandomVertices(10) << "\n"
//          << endl;
//     return 0;
// }