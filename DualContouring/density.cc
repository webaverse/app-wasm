#include "density.h"
#include "../glm/ext.hpp"

#include "../cachedNoise.h"
#include "../biomes.h"

float sphere(const glm::vec3 &worldPosition, const glm::vec3 &origin, float radius)
{
	return length(worldPosition - origin) - radius;
}

float cuboid(const glm::vec3 &worldPosition, const glm::vec3 &origin, const glm::vec3 &halfDimensions)
{
	const glm::vec3 &local_pos = worldPosition - origin;
	const glm::vec3 &pos = local_pos;

	const glm::vec3 &d = glm::abs(pos) - halfDimensions;
	const float m = glm::max(d.x, glm::max(d.y, d.z));
	return glm::min(m, glm::length(glm::max(d, glm::vec3(0.f))));
}

float falloffMap(const glm::vec2 &position)
{
	return (1 / ((position.x * position.y) * (1 - (position.x / 1)) * (1 - position.y)));
}

float FBM3(const glm::vec3 &position)
{
	const int octaves = 4;
	const float frequency = 0.54;
	const float lacunarity = 2.24;
	const float persistence = 0.68;
	const float SCALE = 1.f / 128.f;
	glm::vec3 p = position * SCALE;
	float noise = 0.f;

	float amplitude = 1.f;
	p *= frequency;

	for (int i = 0; i < octaves; i++)
	{
		noise += simplex(p) * amplitude;
		p *= lacunarity;
		amplitude *= persistence;
	}

	// move into [0, 1] range
	return 0.5f + (0.5f * noise);
}
float FBM2(const glm::vec2 &position)
{
	const int octaves = 4;
	const float frequency = 0.54;
	const float lacunarity = 2.24;
	const float persistence = 0.68;
	const float SCALE = 1.f / 128.f;
	glm::vec2 p = position * SCALE;
	float noise = 0.f;

	float amplitude = 1.f;
	p *= frequency;

	for (int i = 0; i < octaves; i++)
	{
		noise += simplex(p) * amplitude;
		p *= lacunarity;
		amplitude *= persistence;
	}

	// move into [0, 1] range
	return 0.5f + (0.5f * noise);
}

float warpedNoise(const glm::vec3 &position)
{
	const float q = FBM3(glm::vec3(position.x + 5.3f, position.y + 0.8, position.z)) * 80.0;
	return FBM3(glm::vec3(position.x + q, position.y + q, position.z + q));
}

float temperatureNoise(const glm::vec3 &position)
{
	float a = warpedNoise(position - glm::vec3(100.0));
	return a;
	// return a - falloffMap(glm::vec2(position.x - 100, position.y + 100));
}
float humidityNoise(const glm::vec3 &position)
{
	float a = warpedNoise(position + glm::vec3(100.0, 20.0, 40.0));
	return a;
	// return a - falloffMap(glm::vec2(position.x + 100, position.y - 100));
}

float fractalNoise(
	const int octaves,
	const float frequency,
	const float lacunarity,
	const float persistence,
	const glm::vec2 &position)
{
	const float SCALE = 1.f / 128.f;
	glm::vec2 p = position * SCALE;
	float noise = 0.f;

	float amplitude = 1.f;
	p *= frequency;

	for (int i = 0; i < octaves; i++)
	{
		noise += simplex(p) * amplitude;
		p *= lacunarity;
		amplitude *= persistence;
	}

	// move into [0, 1] range
	return 0.5f + (0.5f * noise);
}

// double getTemperature(const glm::vec2 &position)
// {
// 	std::mt19937 rng;
// 	CachedNoise temperature(rng(), 0.001, 4);
// 	return temperature.in2D(position.x + 1000, position.y + 1000);
// }

// double getHumidity(const glm::vec2 &position)
// {
// 	std::mt19937 rng;
// 	CachedNoise humidity(rng(), 0.001, 4);
// 	return humidity.in2D(position.x + 1000, position.y + 1000);
// }

unsigned char getBiome(const glm::vec3 &worldPosition)
{
	unsigned char biome = 0xFF;
	const int t = (int)std::floor((double)temperatureNoise(glm::vec3(worldPosition.x + 100, worldPosition.y, worldPosition.z - 100)) * 16.0);
	const int h = (int)std::floor((double)humidityNoise(glm::vec3(worldPosition.x - 100, worldPosition.y, worldPosition.z + 100)) * 16.0);
	biome = (unsigned char)BIOMES_TEMPERATURE_HUMIDITY[t + 16 * h];
	// std::cout << +biome << std::endl;
	return biome;
}

float Density_Func(const glm::vec3 &position)
{
	const float MAX_HEIGHT = 20.f;
	float noise = 0.0;
	const glm::vec2 p = glm::vec2(position.x, position.z);
	const float sim = glm::clamp(glm::simplex(p / (float)200)*5.0 , 0.0 , 1.0);
	const float fbmNoise = FBM2(p);
	const glm::vec2 q = glm::vec2(fbmNoise * 50.0, FBM2(p + glm::vec2(50.2, 1.3)) * 60.0);
	glm::vec2 r = glm::vec2(FBM2(p + q + glm::vec2(-1.7, 90.2)) * 60.0, FBM2(p + q + glm::vec2(80.3, 2.8)) * 50.0);
	noise += glm::clamp((1.0 - sim) * FBM2(p + r)*2.0, -100.0 , 10.0);
	noise += sim * fbmNoise;
	// std::cout << position.x << std::endl;

	// const float mountainsNoise = fractalNoise(2, 0.2f, 2.1f, 0.21f, p);
	const float terrain = position.y + (MAX_HEIGHT * noise);

	const float cube = cuboid(position, glm::vec3(-4., 10.f, -4.f), glm::vec3(12.f));
	// const float sphere = sphere(position, glm::vec3(15.f, 2.5f, 1.f), 16.f);

	return glm::max(-cube, terrain);
	// return cube;
}
