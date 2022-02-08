#include <vector>
#include <math.h>
#include <stdio.h>

#include "chunk.h"
#include "perlin.h"
#include "mc.h"

namespace Terrain {

	float isoLevel = 0.0;

	Chunk::Chunk(float origin[3], float chunkSize) {
		this->origin = {origin[0], origin[1], origin[2]};
		this->unitSize = chunkSize / (float)this->segment;
		this->points.resize((int)pow(this->segment + 1, 3));
		this->build();
	}

	int Chunk::indexFromCoord(int x, int y, int z) {
		int seg = this->segment + 1;
		return z * seg * seg + y * seg + x;
	}

	void Chunk::density(int x, int y, int z) {
		float noiseScale = 2.99;
		int octaves = 6;
		float persistence = 1.0;
        float lacunarity = 1.0;
        float floorOffset = 20.19;
        float hardFloor = -2.84;
        float hardFloorWeight = 3.05;
        float noiseWeight = 6.09;

        Vector3 v1{(float)x * unitSize, (float)y * unitSize, (float)z * unitSize};
        Vector3 curPos{origin.x + v1.x, origin.y + v1.y, origin.z + v1.z};
        Vector3 offsetNoise{0.0, 0.0, 0.0};
        float noise = 0.0;
        float frequency = noiseScale / 140.0;
        float amplitude = 1.0;
        float weight = 1.0;
        float weightMultiplier = 1.0;
        float paramX = 1.0;
        float paramY = 0.1;

        for (int j = 0; j < octaves; j++) {
        	Vector3 v2{curPos.x, curPos.y, curPos.z};
        	v2.x = (v2.x + offsetNoise.x) * frequency;
        	v2.y = (v2.y + offsetNoise.y) * frequency;
        	v2.z = (v2.z + offsetNoise.z) * frequency;

        	float n = (float)Perlin::noise(v2.x, v2.y, v2.z) / 6.88;
        	float v = 1.0 - abs(n);
        	v = v * v;
        	v *= weight;
        	weight = fmax(fmin(v * weightMultiplier, 1.0), 0.0);
        	noise += v * amplitude;
        	amplitude *= persistence;
        	frequency *= lacunarity;
        }

        float finalVal = -(curPos.y * 0.8 + floorOffset) + noise * noiseWeight +
        	(curPos.y - floor(curPos.y / paramX) * paramX) * paramY;

        if (curPos.y < hardFloor) {
            finalVal += hardFloorWeight;
        }

        int index = indexFromCoord(x, y, z);
        this->points[index] = Vector4{curPos.x, curPos.y, curPos.z, finalVal};

	}

	Vector3 Chunk::interpolateVerts(Vector4 v1, Vector4 v2) {
		float t = (isoLevel - v1.w) / (v2.w - v1.w);
		Vector3 _v1{v1.x, v1.y, v1.z};
		Vector3 _v2{v2.x, v2.y, v2.z};

		return {
			_v1.x + (_v2.x - v1.x) * t,
			_v1.y + (_v2.y - v1.y) * t,
			_v1.z + (_v2.z - v1.z) * t,
		};
	}

	void Chunk::march(int x, int y, int z) {

		// 8 corners of the current cube
        Vector4 cubeCorners[8] = {
            this->points[this->indexFromCoord(x, y, z)],
            this->points[this->indexFromCoord(x + 1, y, z)],
            this->points[this->indexFromCoord(x + 1, y, z + 1)],
            this->points[this->indexFromCoord(x, y, z + 1)],
            this->points[this->indexFromCoord(x, y + 1, z)],
            this->points[this->indexFromCoord(x + 1, y + 1, z)],
            this->points[this->indexFromCoord(x + 1, y + 1, z + 1)],
            this->points[this->indexFromCoord(x, y + 1, z + 1)]
        };

        // Calculate unique index for each cube configuration.
        // There are 256 possible values
        // A value of 0 means cube is entirely inside surface; 255 entirely outside.
        // The value is used to look up the edge table, which indicates which edges of the cube are cut by the isosurface.
        int cubeIndex = 0;
        if (cubeCorners[0].w < isoLevel) cubeIndex |= 1;
        if (cubeCorners[1].w < isoLevel) cubeIndex |= 2;
        if (cubeCorners[2].w < isoLevel) cubeIndex |= 4;
        if (cubeCorners[3].w < isoLevel) cubeIndex |= 8;
        if (cubeCorners[4].w < isoLevel) cubeIndex |= 16;
        if (cubeCorners[5].w < isoLevel) cubeIndex |= 32;
        if (cubeCorners[6].w < isoLevel) cubeIndex |= 64;
        if (cubeCorners[7].w < isoLevel) cubeIndex |= 128;
        if (cubeIndex == 0)
            return;

        // Create triangles for current cube configuration
        for (int i = 0; triangulation[cubeIndex][i] != -1; i += 3) {
            // Get indices of corner points A and B for each of the three edges
            // of the cube that need to be joined to form the triangle.
            int a0 = cornerIndexAFromEdge[triangulation[cubeIndex][i]];
            int b0 = cornerIndexBFromEdge[triangulation[cubeIndex][i]];

            int a1 = cornerIndexAFromEdge[triangulation[cubeIndex][i + 1]];
            int b1 = cornerIndexBFromEdge[triangulation[cubeIndex][i + 1]];

            int a2 = cornerIndexAFromEdge[triangulation[cubeIndex][i + 2]];
            int b2 = cornerIndexBFromEdge[triangulation[cubeIndex][i + 2]];

            Vector3 vA = interpolateVerts(cubeCorners[a0], cubeCorners[b0]);
            Vector3 vB = interpolateVerts(cubeCorners[a1], cubeCorners[b1]);
            Vector3 vC = interpolateVerts(cubeCorners[a2], cubeCorners[b2]);
            this->vertices.push_back(vA);
            this->vertices.push_back(vB);
            this->vertices.push_back(vC);
        }

	}

	void Chunk::build() {
		// density

		for (int i = 0; i <= segment; i++) {
			for (int j = 0; j <= segment; j++) {
				for (int k = 0; k <= segment; k++) {
					density(i, j, k);
				}
			}
		}

		// marching cubes

		for (int i = 0; i < segment; i++) {
			for (int j = 0; j < segment; j++) {
				for (int k = 0; k < segment; k++) {
					march(i, j, k);
				}
			}
		}

		// faces

		for (int i = 0; i < this->vertices.size(); i++) {
			this->indices.push_back(i);
		}
	}

	float* Chunk::getGeometryBuffer() {
		int vertexCount = this->vertices.size() * 3;
		int faceCount = this->indices.size();

		float *outputBuffer = (float*)malloc((2 + this->vertices.size() * 3 + this->indices.size()) * 4);

		memcpy(outputBuffer, &vertexCount, 4);
		memcpy(outputBuffer + 1, &faceCount, 4);
		memcpy(outputBuffer + 2, &(this->vertices.front()), this->vertices.size() * 3 * 4);
		memcpy(outputBuffer + 2 + this->vertices.size() * 3, &(this->indices.front()), this->indices.size() * 4);

		return outputBuffer;
	}

	Chunk::~Chunk() {

	}
}
