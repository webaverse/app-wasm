#include <math.h>
#include <vector>
#include <map>
#include <string>

#include "terrain.h"
#include "perlin.h"
#include "mc.h"
#include "../vector.h"

namespace Terrain {

float isoLevel = 0.0;

float noiseScale = 3.0;
int octaves = 8;
float persistence = 1.15;
float lacunarity = 1.6;
float floorOffset = 20.0;
float hardFloor = 2.0;
float hardFloorWeight = 3.06;
float noiseWeight = 6.09;

float* generateTerrain(
    float chunkSize, int chunkCount, int maxSegment, int vertexStrideParam, int faceStrideParam
) {

    // std::vector<Vector3> vertices = {};
    // std::vector<int> indices = {};

    int cellCount = chunkCount * chunkCount * maxSegment * maxSegment;

    int chunkMaxVertexCount = maxSegment * maxSegment * vertexStrideParam;
    int chunkMaxIndexCount = maxSegment * maxSegment * faceStrideParam;


    float *vertexBuffer = (float*)malloc(chunkCount * chunkCount * chunkMaxVertexCount * 3 * sizeof(float));
    float *normalBuffer = (float*)malloc(chunkCount * chunkCount * chunkMaxVertexCount * 3 * sizeof(float));
    int *faceBuffer = (int*)malloc(chunkCount * chunkCount * chunkMaxIndexCount * sizeof(int));
    int *groupBuffer = (int*)malloc(chunkCount * chunkCount * 2 * sizeof(int));

    // std::vector<int> chunkGroup = {};

    for (int i = 0; i < chunkCount; i++) {
        for (int j = 0; j < chunkCount; j++) {
            float origin[3] = {
                (float)(i - chunkCount / 2) * chunkSize,
                0.0,
                (float)(j - chunkCount / 2) * chunkSize
            };

            // int groupStart = indices.size();
            // chunkGroup.push_back(groupStart);
            int chunkIndex = i * chunkCount + j;

            int indexCount = 0;

            createChunk(
                origin,
                chunkSize,
                maxSegment,
                vertexBuffer,
                normalBuffer,
                faceBuffer,
                chunkIndex * chunkMaxVertexCount,
                chunkIndex * chunkMaxIndexCount,
                indexCount
            );

            groupBuffer[chunkIndex * 2] = chunkIndex * chunkMaxIndexCount;
            groupBuffer[chunkIndex * 2 + 1] = indexCount;

            // chunkGroup.push_back(indices.size() - groupStart);
        }
    }

    int *outputBuffer = (int*)malloc(4 * sizeof(int));

    outputBuffer[0] = (int)vertexBuffer;
    outputBuffer[1] = (int)normalBuffer;
    outputBuffer[2] = (int)faceBuffer;
    outputBuffer[3] = (int)groupBuffer;

    return (float*)outputBuffer;
}

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

void density(
    int x, int y, int z, Vector3 origin, float unitSize, int segment, std::vector<Vector4> & points
) {

    Vector3 v1{(float)x * unitSize, (float)y * unitSize, (float)z * unitSize};
    Vector3 curPos{origin.x + v1.x, origin.y + v1.y, origin.z + v1.z};
    Vector3 offsetNoise{0.0, 0.0, 0.0};
    float noise = 0.0;
    float frequency = noiseScale / 2000.0;
    float amplitude = 1.0;
    float weight = 1.05;
    float weightMultiplier = 1.05;
    float paramX = 1.0;
    float paramY = 0.1;

    for (int j = 0; j < octaves; j++) {
    	Vector3 v2{curPos.x, curPos.y, curPos.z};
    	v2.x = (v2.x + offsetNoise.x) * frequency;
    	v2.y = (v2.y + offsetNoise.y) * frequency;
    	v2.z = (v2.z + offsetNoise.z) * frequency;

    	float n = (float)Perlin::noise(v2.x, v2.y, v2.z) / 2;
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

    if (y == 0) {
    	finalVal = 0.1;
    }

    int index = indexFromCoord(x, y, z, segment);
    points[index] = Vector4{curPos.x, curPos.y, curPos.z, finalVal};
}

void march(
	int x, int y, int z, int segment,
	const std::vector<Vector4> & points, float *vertices, float *normals, int *indices,
	std::map<std::string, int> & vertexDic, int & index, int & faceIndex
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

            for (int i = 0; i < 3; i++) {
            	int v[2] = {segs[i][0], segs[i][1]};

            	std::string vInx =
                    std::to_string(std::min(cornerIndices[v[0]], cornerIndices[v[1]])) + "_" +
            		std::to_string(std::max(cornerIndices[v[0]], cornerIndices[v[1]]));

            	int vertexIndex;
            	Vector3 vP;

            	if (vertexDic.find(vInx) != vertexDic.end()) {
            		vertexIndex = vertexDic[vInx];
            	} else {
            		vP = interpolateVerts(cubeCorners[v[0]], cubeCorners[v[1]]);
            		vertexIndex = index;
            		vertexDic[vInx] = vertexIndex;
            		// vertices.push_back(vP);
                    vertices[index * 3] = vP.x;
                    vertices[index * 3 + 1] = vP.y;
                    vertices[index * 3 + 2] = vP.z;

                    index++;
            	}

                triangleVertexInices[i] = vertexIndex;

            	// indices.push_back(vertexIndex);
                indices[faceIndex] = (int)vertexIndex;
                faceIndex++;
            }

            // calculate normals

            Vec v[3];

            for (int i = 0; i < 3; i++) {
                v[i] = Vec(
                    vertices[triangleVertexInices[i] * 3],
                    vertices[triangleVertexInices[i] * 3 + 1],
                    vertices[triangleVertexInices[i] * 3 + 2]
                );
            }

            Vec normal = (v[1] - v[0]) ^ (v[2] - v[1]);

            normal.normalize();

            for (int i = 0; i < 3; i++) {
                normals[triangleVertexInices[i] * 3] = normal.x;
                normals[triangleVertexInices[i] * 3 + 1] = normal.y;
                normals[triangleVertexInices[i] * 3 + 2] = normal.z;
            }

        }
	}


void createChunk(
    float origin[3], float chunkSize, int segment,
    float *vertices, float *normals, int *indices,
    int vertexOffset, int indexOffset, int & indexCount
) {

	// std::vector<Vector3> vertices = {};
	// std::vector<int> indices = {};
	std::vector<Vector4> points = {};
	std::map<std::string, int> vertexDic;

	float unitSize = chunkSize / (float)segment;
	Vector3 originPos{origin[0], origin[1], origin[2]};
	int index = vertexOffset;
    int faceIndex = indexOffset;

	points.resize((int)pow(segment + 1, 3));

	// density

	for (int i = 0; i <= segment; i++) {
		for (int j = 0; j <= segment; j++) {
			for (int k = 0; k <= segment; k++) {
				density(i, j, k, originPos, unitSize, segment, points);
			}
		}
	}

	// march cubes

	for (int i = 0; i < segment; i++) {
		for (int j = 0; j < segment; j++) {
			for (int k = 0; k < segment; k++) {
				march(i, j, k, segment, points, vertices, normals, indices, vertexDic, index, faceIndex);
			}
		}
	}

    indexCount = faceIndex - indexOffset;
}

}  // Terrain namespace
