#ifndef CHACHEDNOISE_H
#define CHACHEDNOISE_H

#include "vectorMath.h"
#include "../FastNoise.h"
#include "../hash.h"
#include <iostream>
#include <algorithm>
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

    double simplex(int x, int z)
    {
        const int index = (x - min.x) + (z - min.z) * size;
        return cachedNoiseField[0];
    }

private:
    int size;
    vm::ivec3 min;
    double cachedNoiseField[64 * 64];
    FastNoise fastNoise;
    std::mt19937 rng;
    double simplexFractal(float x, float y)
    {
        return (1.0 + fastNoise.GetSimplexFractal(x, y)) / 2.0;
    }
    void init()
    {
        fastNoise.SetFrequency(0.456);
        fastNoise.SetFractalOctaves(8);

        for (int dz = 0; dz < size + 1; dz++)
        {
            for (int dx = 0; dx < size + 1; dx++)
            {
                vm::ivec2 pos = vm::ivec2(dx, dz) + vm::ivec2(min.x, min.z);
                cachedNoiseField[dx + dz * size] = simplexFractal(pos.x, pos.y);
            }
        }
    }
};
#endif // CHACHEDNOISE_H