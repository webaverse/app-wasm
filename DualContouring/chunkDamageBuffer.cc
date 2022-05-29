#include "chunkDamageBuffer.h"

template<typename T>
T bilinear( 
    const float &tx, 
    const float &ty, 
    const T &c00, 
    const T &c10, 
    const T &c01, 
    const T &c11) 
{
  T a = c00 * (1.f - tx) + c10 * tx; 
  T b = c01 * (1.f - tx) + c11 * tx; 
  return a * (1.f - ty) + b * ty; 
}
template<typename T>
T trilinear(
  const vm::vec3 &location,
  const std::vector<T> &data,
  int gridPoints
)
{
  vm::ivec3 p000 = vm::ivec3(int(location.x), int(location.y), int(location.z));
  vm::ivec3 p100 = vm::ivec3(int(location.x) + 1, int(location.y), int(location.z));
  vm::ivec3 p010 = vm::ivec3(int(location.x), int(location.y) + 1, int(location.z));
  vm::ivec3 p110 = vm::ivec3(int(location.x) + 1, int(location.y) + 1, int(location.z));
  vm::ivec3 p001 = vm::ivec3(int(location.x), int(location.y), int(location.z) + 1);
  vm::ivec3 p101 = vm::ivec3(int(location.x) + 1, int(location.y), int(location.z) + 1);
  vm::ivec3 p011 = vm::ivec3(int(location.x), int(location.y) + 1, int(location.z) + 1);
  vm::ivec3 p111 = vm::ivec3(int(location.x) + 1, int(location.y) + 1, int(location.z) + 1);

  int i000 = p000.x + p000.z * gridPoints + p000.y * gridPoints * gridPoints;
  int i100 = p100.x + p100.z * gridPoints + p100.y * gridPoints * gridPoints;
  int i010 = p010.x + p010.z * gridPoints + p010.y * gridPoints * gridPoints;
  int i110 = p110.x + p110.z * gridPoints + p110.y * gridPoints * gridPoints;
  int i001 = p001.x + p001.z * gridPoints + p001.y * gridPoints * gridPoints;
  int i101 = p101.x + p101.z * gridPoints + p101.y * gridPoints * gridPoints;
  int i011 = p011.x + p011.z * gridPoints + p011.y * gridPoints * gridPoints;
  int i111 = p111.x + p111.z * gridPoints + p111.y * gridPoints * gridPoints;

  const T &v000 = data.at(i000);
  const T &v100 = data.at(i100);
  const T &v010 = data.at(i010);
  const T &v110 = data.at(i110);
  const T &v001 = data.at(i001);
  const T &v101 = data.at(i101);
  const T &v011 = data.at(i011);
  const T &v111 = data.at(i111);

  float tx = location.x - p000.x;
  float ty = location.y - p000.y;
  float tz = location.z - p000.z;

  const T &e = bilinear<T>(tx, ty, v000, v100, v010, v110); 
  const T &f = bilinear<T>(tx, ty, v001, v101, v011, v111); 
  return e * (1 - tz) + f * tz; 
}

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

float ChunkDamageBuffer::getInterpolated(const float &x, const float &y, const float &z) {
  const float localX = x - min.x + 1;
  const float localY = y - min.y + 1;
  const float localZ = z - min.z + 1;
  return trilinear<float>(
    vm::vec3(localX, localY, localZ),
    cachedDamageField,
    gridPoints
  );
}