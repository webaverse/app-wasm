#ifndef DENSITY_H
#define DENSITY_H

#include "vectorMath.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include "cachedNoise.h"

float Density_Func(const vm::vec3 &worldPosition, CachedNoise &chunkNoise);
unsigned char getBiome(const vm::vec3 &worldPosition, CachedNoise &chunkNoise);

#endif //	DENSITY_H
