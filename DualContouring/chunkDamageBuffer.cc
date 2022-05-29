#include "chunkDamageBuffer.h"

/* ChunkDamageBuffer(const ChunkDamageBuffer &other) {
  min = other.min;
  gridPoints = other.gridPoints;
  cachedDamageField = other.cachedDamageField;
} */
ChunkDamageBuffer::ChunkDamageBuffer(const ChunkDamageBuffer &&other) {
  min = other.min;
  size = other.size;
  gridPoints = other.gridPoints;
  cachedDamageField = std::move(other.cachedDamageField);
}
ChunkDamageBuffer::ChunkDamageBuffer(const vm::ivec3 &min, const int &size) :
  min(min),
  size(size),
  gridPoints(size + 3),
  cachedDamageField(gridPoints * gridPoints * gridPoints)
  {}
/* ChunkDamageBuffer &ChunkDamageBuffer::operator=(const ChunkDamageBuffer &other) {
  min = other.min;
  gridPoints = other.gridPoints;
  cachedDamageField = other.cachedDamageField;
  return *this;
} */
ChunkDamageBuffer &ChunkDamageBuffer::operator=(ChunkDamageBuffer &&other) {
  min = other.min;
  size = other.size;
  gridPoints = other.gridPoints;
  cachedDamageField = std::move(other.cachedDamageField);
  return *this;
}

//

bool ChunkDamageBuffer::drawDamage(const float &x, const float &y, const float &z, const float radius, const float value) {
  int ax = int(x) - min.x + 1;
  int az = int(z) - min.z + 1;
  int ay = int(y) - min.y + 1;
  bool drew = false;
  for (float dx = -radius; dx <= radius; dx++) {
    for (float dz = -radius; dz <= radius; dz++) {
      for (float dy = -radius; dy <= radius; dy++) {
        int lx = ax + dx;
        int lz = az + dz;
        int ly = ay + dy;
        if (
          lx >= 0 && lx < gridPoints &&
          lz >= 0 && lz < gridPoints &&
          ly >= 0 && ly < gridPoints
        ) {
          float distance = sqrt(dx * dx + dy * dy + dz * dz);
          float distanceFactor = 1 - distance / radius;
          int index = lx + lz * gridPoints + ly * gridPoints * gridPoints;
          
          cachedDamageField[index] += value * distanceFactor;
          drew = true;
        }
      }
    }
  }
  return drew;
}

float ChunkDamageBuffer::getInterpolated(const float &x, const float &z) {
  const float localX = x - min.x + 1;
  const float localZ = z - min.z + 1;
  return interpolate2D(localX, localZ);
}