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

    float getInterpolated(const float &x, const float &z)
    {
        const float localX = x - min.x + 1;
        const float localZ = z - min.z + 1;
        return interpolate2D(localX, localZ);
        // return (float)fastNoise.GetSimplexFractal(x,z);
    }
    float getRaw(const float &x, const float &z)
    {
        const int localX = std::round(x) - min.x + 1;
        const int localZ = std::round(z) - min.z + 1;
        const int index = localX + localZ * size;
        return cachedNoiseField.at(index);
    }

private:
    int size;
    vm::ivec3 min;
    std::vector<float> cachedNoiseField;
    std::mt19937 rng;
    FastNoise fastNoise;

    void init()
    {
        fastNoise.SetFrequency(0.02);
        fastNoise.SetFractalOctaves(4);

        cachedNoiseField.resize((size + 3) * (size + 3));

        for (int dz = 0; dz < size + 3; dz++)
            for (int dx = 0; dx < size + 3; dx++)
                cachedNoiseField[dx + dz * size] = (float)fastNoise.GetSimplexFractal(dx + min.x - 1, dz + min.z - 1);
    }
    float lerp(const float &a, const float &b, const float &f)
    {
        return a + f * (b - a);
    }
    float interpolate(const float &x, const float &z)
    {
        const int xf = std::floor(x);
        const int xc = std::ceil(x);
        const int indexF = xf + z * size;
        const int indexC = xc + z * size;
        const float dx = x - xf;
        return lerp(cachedNoiseField.at(indexF), cachedNoiseField.at(indexC), dx);
        // return lerp(cachedNoiseField[indexF], cachedNoiseField[indexC], dx);
    }
    float interpolate2D(const float &x, const float &z)
    {
        const int zf = std::floor(z);
        const int zc = std::ceil(z);
        const float dz = z - zf;
        return lerp(interpolate(z, zf), interpolate(x, zc), dz);
    }
};
#endif // CHACHEDNOISE_H