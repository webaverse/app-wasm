#include <vector>
#include "../FastNoise.h"
namespace DualContouring
{
    struct Vector3
    {
        float x;
        float y;
        float z;
    };

    struct Vector4
    {
        float x;
        float y;
        float z;
        float w;
    };
    
    int *generateVertices(int segment);
}