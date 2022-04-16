#include <vector>
#include <cstdint>
#include <ctime>
#include <string.h>
#include "../FastNoise.h"

using namespace std;
namespace DualContouring
{
    struct vec3
    {
        float x;
        float y;
        float z;
    };

    struct vec4
    {
        float x;
        float y;
        float z;
        float w;
    };

    vector<vec3> generateVertices(int width, int height, int depth);
}