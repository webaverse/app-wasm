#include "density.h"
#include "../biomes.h"

float sphere(const vm::vec3 &worldPosition, const vm::vec3 &origin, float radius)
{
	return vm::length(worldPosition - origin) - radius;
}

float cuboid(const vm::vec3 &worldPosition, const vm::vec3 &origin, const vm::vec3 &halfDimensions)
{
	const vm::vec3 &local_pos = worldPosition - origin;
	const vm::vec3 &pos = local_pos;

	const vm::vec3 &d = vm::abs(pos) - halfDimensions;
	const float m = std::max(d.x, std::max(d.y, d.z));
	return std::min(m, vm::length(vm::max(d, 0.0)));
}

// float falloffMap(const vm::vec2 &position)
// {
// 	return (1 / ((position.x * position.y) * (1 - (position.x / 1)) * (1 - position.y)));
// }

// template <typename T>
// float FBM(const T &position)
// {
// 	const int octaves = 4;
// 	const float frequency = 0.54;
// 	const float lacunarity = 2.24;
// 	const float persistence = 0.68;
// 	const float SCALE = 1.f / 128.f;
// 	T p = position * SCALE;
// 	float noise = 0.f;

// 	float amplitude = 1.f;
// 	p = p * frequency;

// 	for (int i = 0; i < octaves; i++)
// 	{
// 		noise += glm::simplex(glm::vec2(p.x, p.y)) * amplitude;
// 		p = p * lacunarity;
// 		amplitude *= persistence;
// 	}

// 	// move into [0, 1] range
// 	return 0.5f + (0.5f * noise);
// }

// float warpedNoise(const vm::vec3 &position)
// {
// 	const float q = FBM(vm::vec3(position.x + 5.3f, position.y + 0.8, position.z)) * 80.0;
// 	return FBM(vm::vec3(position.x + q, position.y + q, position.z + q));
// }

// float temperatureNoise(const vm::vec3 &position)
// {
// 	const vm::vec3 p = position / 5.f;
// 	float a = warpedNoise(p - vm::vec3(100.0));
// 	return a;
// 	// return a - falloffMap(vm::vec2(position.x - 100, position.y + 100));
// }
// float humidityNoise(const vm::vec3 &position)
// {
// 	const vm::vec3 p = position / 5.f;
// 	float a = warpedNoise(p + vm::vec3(100.0, 20.0, 40.0));
// 	return a;
// 	// return a - falloffMap(vm::vec2(position.x + 100, position.y - 100));
// }

unsigned char getBiome(const vm::ivec2 &worldPosition, CachedNoise &chunkNoise)
{
	unsigned char biome = 0xFF;
	const float noiseValue = chunkNoise.getRaw(worldPosition.x, worldPosition.y);
	const int t = (int)std::floor(noiseValue * 16.0);
	const int h = (int)std::floor(noiseValue * 16.0);
	biome = (unsigned char)BIOMES_TEMPERATURE_HUMIDITY[t + 16 * h];

	return biome;
}

float Density_Func(const vm::vec3 &position, CachedNoise &chunkNoise, ChunkDamageBuffer &damageBuffer)
{
	const float MAX_HEIGHT = 20.f;
	// float noise = 0.0;
	const vm::vec2 p = vm::vec2(position.x, position.z);
	// const float mask = glm::clamp(glm::simplex(glm::vec2(p.x, p.y) / (float)200) * 5.0, 0.0, 1.0);
	// const float fbmNoise = FBM(p);
	// const vm::vec2 q = vm::vec2(fbmNoise * 50.0, FBM(p + vm::vec2(50.2, 1.3)) * 60.0);
	// noise += glm::clamp((1.0 - mask) * FBM(p + q) * 2.0, -100.0, 10.0);
	// noise += mask * fbmNoise / 2.0;
	const float noise = chunkNoise.getInterpolated(p.x, p.y);
	// std::cout << noise << std::endl;
	const float damage = damageBuffer.getInterpolated(p.x, p.y, p.z);

	const float terrain = position.y +
	  (MAX_HEIGHT * noise) +
		damage;
	// std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;

	// const float mountainsNoise = fractalNoise(2, 0.2f, 2.1f, 0.21f, p);

	const float cube = cuboid(position, vm::vec3(-4., 10.f, -4.f), vm::vec3(12.f));
	// const float orb = sphere(position, vm::vec3(15.f, 2.5f, 1.f), 16.f);

	// return orb;
	// return cube;
	// return terrain;
	return std::max(-cube, terrain);
}
