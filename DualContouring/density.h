#ifndef DENSITY_H
#define DENSITY_H

#include "../glm/glm.hpp"
#include <functional>
using namespace glm;

class DensityData
{
    std::function<float(float, float)> m_heightFunc;
    DensityData(){}
public:
    static DensityData& GetInstance(){
        static DensityData instance;
        return instance;
    }

    void SetHeightFunction(std::function<float(float, float)> heightFunc)
    {
        m_heightFunc = heightFunc;
    }

    std::function<float(float, float)> GetHeightFunction()
    {
        return m_heightFunc;
    }
};

float Density_Func(const vec3 &worldPosition);

#endif //	DENSITY_H
