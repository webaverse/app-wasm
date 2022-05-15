#ifndef DENSITY_H
#define DENSITY_H

#include <iostream>
#include "../glm/glm.hpp"
// #include "../cachedNoise.h"

float Density_Func(const glm::vec3 &worldPosition);
unsigned char getBiome(const glm::vec3 &worldPosition);

#endif //	DENSITY_H
