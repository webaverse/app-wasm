#ifndef CHACHEDNOISE_H
#define CHACHEDNOISE_H

#include "vectorMath.h"
#include "../FastNoise.h"
#include "../hash.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <vector>

class CachedNoise
{
public:
    CachedNoise() : rng(1000),
                    fastNoise(rng()), min(vm::ivec3(-32)), size(64)
    {
        init();
    };
    CachedNoise(const vm::ivec3 chunkMin, int chunkSize) : rng(100),
                                                           fastNoise(rng()), min(chunkMin), size(chunkSize)
    {
        init();
    };

    double simplex(const float &x, const float &z)
    {
        // const int index = (x - min.x + 1) + (z - min.z + 1) * size;
        // return cachedNoiseField[index];
        return interpolate2D(x, z);
    }

private:
    int size;
    vm::ivec3 min;
    double cachedNoiseField[(64 + 1) * (64 + 1)];
    FastNoise fastNoise;
    std::mt19937 rng;
    double simplexFractal(float x, float y)
    {
        return (1.0 + fastNoise.GetSimplexFractal(x, y)) / 2.0;
    }
    void init()
    {
        fastNoise.SetFrequency(0.02);
        fastNoise.SetFractalOctaves(4);

        for (int dx = 0; dx < size + 1; dx++)
            for (int dz = 0; dz < size + 1; dz++)
            {
                {
                    vm::ivec2 pos = vm::ivec2(dx, dz) + vm::ivec2(min.x, min.z);
                    cachedNoiseField[dx + dz * size] = simplexFractal(pos.x, pos.y);
                }
            }
    }
    double lerp(const double &a, const double &b, const double &f)
    {
        return a + f * (b - a);
    }
    double interpolate(const float &x, const float &y)
    {
        const int xf = std::floor(x);
        const float dx = x - xf;
        const int indexF = xf + y * size;

        // if (indexF < 0 || indexF > (64 + 1) * (64 + 1))
        // {
        //     std::cout << "X : " << x << ", Z: " << y << std::endl;
        //     return 0.5;
        // }

        if (dx == 0)
        {
            return cachedNoiseField[indexF];
        }

        const int xc = std::ceil(x);
        const int indexC = xc + y * size;

        // if (indexC < 0 || indexC > (64 + 1) * (64 + 1))
        // {
        //     std::cout << "X : " << x << ", Z: " << y << std::endl;
        //     return 0.5;
        // }

        if (dx == 1)
        {
            return cachedNoiseField[indexC];
        }

        const double r1 = cachedNoiseField[indexF];
        const double r2 = cachedNoiseField[indexC];

        return lerp(r1, r2, dx);
    }
    double interpolate2D(const float &x, const float &z)
    {
        float x1 = x - min.x;
        float z1 = z - min.z;
        // if(x1 < 0){
        //     x1 = 0;
        // }
        // if(z1 < 0){
        //     z1 = 0;
        // }
        // const float x1 = vm::clamp(x - min.x , 0.0 , 65.0);
        // const float z1 = vm::clamp(z - min.z , 0.0 , 65.0);

        const int zf = std::floor(z1);

        const float dz = z - zf;

        if (dz == 0)
        {
            const int indexF = x1 + zf * size;
            return cachedNoiseField[indexF];
        }

        const int zc = std::ceil(z1);

        if (dz == 1)
        {
            const int indexC = x1 + zc * size;
            return cachedNoiseField[indexC];
        }

        const double yf = interpolate(x1, zf);
        const double yc = interpolate(x1, zc);
        // std::cout << interpolate(y1, y2) << std::endl;
        return lerp(yf, yc, dz);
    }
};
#endif // CHACHEDNOISE_H