#ifndef _CHUNK_DAMAGE_BUFFER_H_
#define _CHUNK_DAMAGE_BUFFER_H_

#include <vector>
#include "vectorMath.h"
#include "cachedNoise.h"

class ChunkDamageBuffer {
public:
  vm::ivec3 min;
  int size;
  int gridPoints;
  std::vector<float> cachedDamageField;

  //

  ChunkDamageBuffer() = delete;
  ChunkDamageBuffer(const ChunkDamageBuffer &other) = delete;
  ChunkDamageBuffer(const ChunkDamageBuffer &&other);
  ChunkDamageBuffer(const vm::ivec3 &min, const int &size);
  ChunkDamageBuffer &operator=(const ChunkDamageBuffer &other) = delete;
  ChunkDamageBuffer &operator=(ChunkDamageBuffer &&other);

  //

  bool drawDamage(const float &x, const float &y, const float &z, const float radius, const float value);
  float getInterpolated(const float &x, const float &y, const float &z);
  float lerp(const float &a, const float &b, const float &f) {
    return a + f * (b - a);
  }
  float interpolate1D(const float &x, const float &z) {
    const int xf = std::floor(x);
    const int xc = std::ceil(x);
    const int indexF = xf + z * gridPoints;
    const int indexC = xc + z * gridPoints;
    const float dx = x - xf;
    return lerp(cachedDamageField.at(indexF), cachedDamageField.at(indexC), dx);
  }
  float interpolate2D(const float &x, const float &z) {
    const int zf = std::floor(z);
    const int zc = std::ceil(z);
    const float dz = z - zf;
    return lerp(interpolate1D(x, zf), interpolate1D(x, zc), dz);
  }
};

#endif