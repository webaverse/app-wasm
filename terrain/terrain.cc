#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "terrain.h"
#include "mc.h"
#include "../vector.h"
#include "../noiser.h"

namespace Terrain {

float isoLevel = 0.0;

int NUM_BUFFER = 6;

int* generateChunk(float x, float y, float z, float chunkSize, int segment) {
    std::vector<Vector3> vertices{};
    std::vector<Vector3> normals{};
    std::vector<VertexBiome> biomes{};
    std::vector<int> indices{};

    float origin[3] = {x, y, z};

    createChunk(
        origin,
        chunkSize,
        segment,
        vertices,
        normals,
        biomes,
        indices
    );

    int *result = (int*)malloc(NUM_BUFFER * sizeof(int));
    float *chunkPositionBuffer = (float*)malloc(vertices.size() * sizeof(Vector3));
    float *chunkNormalBuffer = (float*)malloc(normals.size() * sizeof(Vector3));
    float *chunkBiomeBuffer = (float*)malloc(biomes.size() * sizeof(VertexBiome));
    int *chunkIndexBuffer = (int*)malloc(indices.size() * sizeof(int));

    memcpy(chunkPositionBuffer, &(vertices.front()), vertices.size() * sizeof(Vector3));
    memcpy(chunkNormalBuffer, &(normals.front()), normals.size() * sizeof(Vector3));
    memcpy(chunkBiomeBuffer, &(biomes.front()), biomes.size() * sizeof(VertexBiome));
    memcpy(chunkIndexBuffer, &(indices.front()), indices.size() * sizeof(int));


    result[0] = vertices.size();
    result[1] = indices.size();
    result[2] = (int)chunkPositionBuffer;
    result[3] = (int)chunkNormalBuffer;
    result[4] = (int)chunkBiomeBuffer;
    result[5] = (int)chunkIndexBuffer;

    return result;
}

/*
 * Generate a chunk
 */

int indexFromCoord(int x, int y, int z, int segment) {
	int seg = segment + 1;
	return z * seg * seg + y * seg + x;
}

Vector3 interpolateVerts(Vector4 v1, Vector4 v2) {
	float t = (isoLevel - v1.w) / (v2.w - v1.w);
	Vector3 _v1{v1.x, v1.y, v1.z};
	Vector3 _v2{v2.x, v2.y, v2.z};

	return {
		_v1.x + (_v2.x - v1.x) * t,
		_v1.y + (_v2.y - v1.y) * t,
		_v1.z + (_v2.z - v1.z) * t,
	};
}

void march(
	int x, int y, int z, int segment, Vector3 chunkMin, Vector3 chunkMax,
	const std::vector<Vector4> & points,
    std::vector<Vector3> & vertices, std::vector<Vector3> & normals, std::vector<VertexBiome> & biomes, std::vector<int> & indices,
	std::map<std::string, int> & vertexDic, int & index, float biome[8]
) {

	// 8 corners of the current cube
    Vector4 cubeCorners[8] = {
        points[indexFromCoord(x, y, z, segment)],
        points[indexFromCoord(x + 1, y, z, segment)],
        points[indexFromCoord(x + 1, y, z + 1, segment)],
        points[indexFromCoord(x, y, z + 1, segment)],
        points[indexFromCoord(x, y + 1, z, segment)],
        points[indexFromCoord(x + 1, y + 1, z, segment)],
        points[indexFromCoord(x + 1, y + 1, z + 1, segment)],
        points[indexFromCoord(x, y + 1, z + 1, segment)]
    };

    int cornerIndices[8] = {
    	indexFromCoord(x, y, z, segment),
        indexFromCoord(x + 1, y, z, segment),
        indexFromCoord(x + 1, y, z + 1, segment),
        indexFromCoord(x, y, z + 1, segment),
        indexFromCoord(x, y + 1, z, segment),
        indexFromCoord(x + 1, y + 1, z, segment),
        indexFromCoord(x + 1, y + 1, z + 1, segment),
        indexFromCoord(x, y + 1, z + 1, segment)
    };

    // Calculate unique index for each cube configuration.
    // There are 256 possible values
    // A value of 0 means cube is entirely inside surface; 255 entirely outside.
    // The value is used to look up the edge table, which indicates which edges
    // of the cube are cut by the isosurface.
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

        int segs[3][2] = {{a0, b0}, {a2, b2}, {a1, b1}};

        int triangleVertexInices[3] = {0, 0, 0};
        Vec triangleVertices[3];

        for (int i = 0; i < 3; i++) {
        	int v[2] = {segs[i][0], segs[i][1]};

        	std::string vInx =
                std::to_string(std::min(cornerIndices[v[0]], cornerIndices[v[1]])) + "_" +
        		std::to_string(std::max(cornerIndices[v[0]], cornerIndices[v[1]]));

        	int vertexIndex;
        	Vector3 vP;

        	if (vertexDic.find(vInx) != vertexDic.end()) {
        		vertexIndex = vertexDic[vInx];
                vP = vertices[vertexIndex];
        	} else {
        		vP = interpolateVerts(cubeCorners[v[0]], cubeCorners[v[1]]);

                if (
                    vP.x > chunkMin.x && vP.x < chunkMax.x &&
                    vP.y > chunkMin.y && vP.y < chunkMax.y &&
                    vP.z > chunkMin.z && vP.z < chunkMax.z
                ) {
                    vertexIndex = index;
                    vertexDic[vInx] = vertexIndex;
                    vertices.push_back(vP);
                    normals.push_back({0.0, 0.0, 0.0});
                    biomes.push_back({
                        biome[0], biome[1], biome[2], biome[3], biome[4], biome[5], biome[6], biome[7]
                    });

                    index++;
                } else {
                    vertexIndex = -1;
                }

        	}

            triangleVertexInices[i] = vertexIndex;
            triangleVertices[i] = Vec(vP.x, vP.y, vP.z);
        }

        if (
            triangleVertexInices[0] != -1 &&
            triangleVertexInices[1] != -1 &&
            triangleVertexInices[2] != -1
        ) {
            indices.push_back(triangleVertexInices[0]);
            indices.push_back(triangleVertexInices[1]);
            indices.push_back(triangleVertexInices[2]);
        }

        // calculate normals

        normals.resize(vertices.size());

        // Vec v[3];

        // for (int i = 0; i < 3; i++) {
        //     v[i] = Vec(
        //         vertices[triangleVertexInices[i]].x,
        //         vertices[triangleVertexInices[i]].y,
        //         vertices[triangleVertexInices[i]].z
        //     );
        // }

        Vec normal = (triangleVertices[1] - triangleVertices[0]) ^ (triangleVertices[2] - triangleVertices[1]);

        normal.normalize();

        for (int i = 0; i < 3; i++) {
            if (triangleVertexInices[i] != -1) {
                normals[triangleVertexInices[i]].x += normal.x;
                normals[triangleVertexInices[i]].y += normal.y;
                normals[triangleVertexInices[i]].z += normal.z;
            }
        }
    }
}

void createChunk(
    float origin[3], float chunkSize, int segment,
    std::vector<Vector3> & vertices, std::vector<Vector3> & normals, std::vector<VertexBiome> & vertexBiomes, std::vector<int> & indices
) {
    Noiser *noiser = new Noiser(0);

    // int pointCount = (segment + 3) * (segment + 3);

    // unsigned char biomes[(segment + 3) * (segment + 3)];
    unsigned char temperature;
    unsigned char humidity;
    float elevations[(segment + 3) * (segment + 3)];
    float ether[(segment + 3) * (segment + 3) * (segment + 3)];

    float unitSize = chunkSize / (float)segment;

    int ox = (int)round(origin[0] / chunkSize);
    int oy = (int)round(origin[1] / chunkSize);
    int oz = (int)round(origin[2] / chunkSize);

    // noiser->fillBiomes(ox, oz, segment, biomes, &temperature, &humidity);

    float biomes[8 * (segment + 3) * (segment + 3)];

    noiser->fillElevations(ox, oz, segment, elevations, biomes);
    noiser->fillEther(ox, oy, oz, segment, unitSize, elevations, ether);

    delete noiser;

	std::vector<Vector4> points = {};
	std::map<std::string, int> vertexDic;

	Vector3 originPos{origin[0], origin[1], origin[2]};
	int index = 0;

	points.resize((int)pow(segment + 3, 3));

    bool validChunk = false;

    int initialPotential = 0;

	// density

	for (int i = 0; i <= segment + 2; i++) {
		for (int j = 0; j <= segment + 2; j++) {
			for (int k = 0; k <= segment + 2; k++) {
				// density(i, j, k, originPos, unitSize, segment, points);

                Vector3 curPos{
                    originPos.x + (i - 1) * unitSize, originPos.y + (j - 1) * unitSize, originPos.z + (k - 1) * unitSize
                };

                int pointIndex = indexFromCoord(i, j, k, segment + 2);

                // float potential = elevations[k * (segment + 3) + i] - curPos.y;
                float potential = -ether[j * (segment + 3) * (segment + 3) + k * (segment + 3) + i];

                if (initialPotential == 0 && abs(potential) > 1e-7) {
                    initialPotential = potential > 0 ? 1 : -1;
                }

                if ((potential > 0 && initialPotential == -1) || (potential < 0 && initialPotential == 1)) {
                    validChunk = true;
                }

                points[pointIndex] = Vector4{
                    curPos.x, curPos.y, curPos.z, potential
                };
			}
		}
	}

    if (!validChunk) {
        return;
    }

    float epsilon = chunkSize * 1e-4;

    Vector3 chunkMin{origin[0] - epsilon, origin[1] - epsilon, origin[2] - epsilon};
    Vector3 chunkMax{origin[0] + chunkSize + epsilon, origin[1] + chunkSize + epsilon, origin[2] + chunkSize + epsilon};

	// march cubes

	for (int i = 0; i < segment + 2; i++) {
		for (int j = 0; j < segment + 2; j++) {
			for (int k = 0; k < segment + 2; k++) {
                float *biome;
                biome = biomes + 8 * (k * (segment + 3) + i);
				march(
                    i, j, k, segment + 2, chunkMin, chunkMax,
                    points, vertices, normals, vertexBiomes, indices, vertexDic, index,
                    biome
                );
                    // biomes[k * (segment + 1) + i]);
			}
		}
	}

    // normalize summed vertex normals

    for (int i = 0; i < normals.size(); i++) {
        Vector3 n = normals[i];
        float length = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        normals[i] = {n.x / length, n.y / length, n.z / length};
    }
}

}  // Terrain namespace
