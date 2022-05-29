#ifndef DENSITY_H
#define DENSITY_H

#include "vectorMath.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include "cachedNoise.h"
#include "chunkDamageBuffer.h"

float Density_Func(const vm::vec3 &worldPosition, CachedNoise &chunkNoise, ChunkDamageBuffer &damageBuffer);
unsigned char getBiome(const vm::ivec2 &worldPosition, CachedNoise &chunkNoise);

#endif //	DENSITY_H
