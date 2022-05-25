#include "octree.h"
#include "density.h"

#define PI 3.14159265358979323846

const int MATERIAL_AIR = 0;
const int MATERIAL_SOLID = 1;

const float QEF_ERROR = 1e-6f;
const int QEF_SWEEPS = 4;

const vm::ivec3 CHILD_MIN_OFFSETS[] =
	{
		// needs to match the vertMap from Dual Contouring impl
		vm::ivec3(0, 0, 0),
		vm::ivec3(0, 0, 1),
		vm::ivec3(0, 1, 0),
		vm::ivec3(0, 1, 1),
		vm::ivec3(1, 0, 0),
		vm::ivec3(1, 0, 1),
		vm::ivec3(1, 1, 0),
		vm::ivec3(1, 1, 1),
};
// data from the original DC impl, drives the contouring process

const int edgevmap[12][2] =
	{
		{0, 4}, {1, 5}, {2, 6}, {3, 7}, // x-axis
		{0, 2},
		{1, 3},
		{4, 6},
		{5, 7}, // y-axis
		{0, 1},
		{2, 3},
		{4, 5},
		{6, 7} // z-axis
};

const int edgemask[3] = {5, 3, 6};

const int vertMap[8][3] =
	{
		{0, 0, 0},
		{0, 0, 1},
		{0, 1, 0},
		{0, 1, 1},
		{1, 0, 0},
		{1, 0, 1},
		{1, 1, 0},
		{1, 1, 1}};

const int faceMap[6][4] = {{4, 8, 5, 9}, {6, 10, 7, 11}, {0, 8, 1, 10}, {2, 9, 3, 11}, {0, 4, 2, 6}, {1, 5, 3, 7}};
const int cellProcFaceMask[12][3] = {{0, 4, 0}, {1, 5, 0}, {2, 6, 0}, {3, 7, 0}, {0, 2, 1}, {4, 6, 1}, {1, 3, 1}, {5, 7, 1}, {0, 1, 2}, {2, 3, 2}, {4, 5, 2}, {6, 7, 2}};
const int cellProcEdgeMask[6][5] = {{0, 1, 2, 3, 0}, {4, 5, 6, 7, 0}, {0, 4, 1, 5, 1}, {2, 6, 3, 7, 1}, {0, 2, 4, 6, 2}, {1, 3, 5, 7, 2}};

const int faceProcFaceMask[3][4][3] = {
	{{4, 0, 0}, {5, 1, 0}, {6, 2, 0}, {7, 3, 0}},
	{{2, 0, 1}, {6, 4, 1}, {3, 1, 1}, {7, 5, 1}},
	{{1, 0, 2}, {3, 2, 2}, {5, 4, 2}, {7, 6, 2}}};

const int faceProcEdgeMask[3][4][6] = {
	{{1, 4, 0, 5, 1, 1}, {1, 6, 2, 7, 3, 1}, {0, 4, 6, 0, 2, 2}, {0, 5, 7, 1, 3, 2}},
	{{0, 2, 3, 0, 1, 0}, {0, 6, 7, 4, 5, 0}, {1, 2, 0, 6, 4, 2}, {1, 3, 1, 7, 5, 2}},
	{{1, 1, 0, 3, 2, 0}, {1, 5, 4, 7, 6, 0}, {0, 1, 5, 0, 4, 1}, {0, 3, 7, 2, 6, 1}}};

const int edgeProcEdgeMask[3][2][5] = {
	{{3, 2, 1, 0, 0}, {7, 6, 5, 4, 0}},
	{{5, 1, 4, 0, 1}, {7, 3, 6, 2, 1}},
	{{6, 4, 2, 0, 2}, {7, 5, 3, 1, 2}},
};

const int processEdgeMask[3][4] = {{3, 2, 1, 0}, {7, 5, 6, 4}, {11, 10, 9, 8}};

vm::vec4 selectFourMostCommonMembers(unsigned char biomeSamples[], const int &n)
{
	vm::vec4 selection;
	std::vector<unsigned char> sortedSamples;
	sortedSamples.reserve(n);
	for (int i = 0; i < n; ++i)
	{
		sortedSamples.emplace_back(biomeSamples[i]);
	}

	std::unordered_map<unsigned char, size_t> count;

	for (unsigned char i : sortedSamples)
	{
		count[i]++;
	}

	std::sort(
		sortedSamples.begin(),
		sortedSamples.end(),
		[&count](unsigned char const &a, unsigned char const &b)
		{
			if (a == b)
			{
				return false;
			}
			if (count[a] > count[b])
			{
				return true;
			}
			else if (count[a] < count[b])
			{
				return false;
			}
			return a < b;
		});

	selection.x = sortedSamples[0];
	selection.y = sortedSamples[1];
	selection.z = sortedSamples[2];
	selection.w = sortedSamples[3];

	return selection;
}

vm::vec4 getBiomesData(const vm::vec3 &position, CachedNoise &chunkNoise)
{
	const int numberOfSamples = 9;
	unsigned char samples[numberOfSamples];

	// sample points on a circle
	float angle = 0.0;
	for (int i = 0; i < numberOfSamples; i++)
	{
		angle = (2 * PI / numberOfSamples) * i;
		const vm::vec3 samplePosition = position + vm::vec3(std::cos(angle), 0.0, std::sin(angle));
		samples[i] = getBiome(position, chunkNoise);
	}

	vm::vec4 biome = selectFourMostCommonMembers(samples, numberOfSamples);

	return biome;
}

vm::vec4 getBiomesWeight(const vm::vec3 &position, const vm::vec4 &biome, CachedNoise &chunkNoise)
{
	vm::vec4 biomesWeights;
	// float biomesValues[4] = {biome.x, biome.y, biome.z, biome.w};
	unsigned char pointBiome = getBiome(position, chunkNoise);
	const float rightDistance = std::abs(+pointBiome - biome.x);
	const float upDistance = std::abs(+pointBiome - biome.y);
	const float leftDistance = std::abs(+pointBiome - biome.z);
	const float downDistance = std::abs(+pointBiome - biome.w);

	const float rightDownWeight = std::sqrt(std::pow(rightDistance, 2) + std::pow(downDistance, 2));
	const float rightUpWeight = std::sqrt(std::pow(rightDistance, 2) + std::pow(upDistance, 2));
	const float leftDownWeight = std::sqrt(std::pow(leftDistance, 2) + std::pow(downDistance, 2));
	const float leftUpWeight = std::sqrt(std::pow(leftDistance, 2) + std::pow(upDistance, 2));

	biomesWeights.x = rightDownWeight;
	biomesWeights.y = rightUpWeight;
	biomesWeights.z = leftDownWeight;
	biomesWeights.w = leftUpWeight;
	// int n = 4;
	// biomesWeights.x = getPercentageInArray(biomesValues, biomesValues[0], n);
	// biomesWeights.y = getPercentageInArray(biomesValues, biomesValues[1], n);
	// biomesWeights.z = getPercentageInArray(biomesValues, biomesValues[2], n);
	// biomesWeights.w = getPercentageInArray(biomesValues, biomesValues[3], n);
	// std::cout << rightDownWeight << std::endl;
	return biomesWeights;
}

OctreeNode *switchChunkLod(OctreeNode *node, LodLevel lod)
{
	if (!node)
	{
		return NULL;
	}

	if (node->type != Node_Internal)
	{
		// can't simplify!
		return node;
	}

	svd::QefSolver qef;
	int signs[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
	int midsign = -1;
	int edgeCount = 0;
	bool isCollapsible = true;

	for (int i = 0; i < 8; i++)
	{
		node->children[i] = switchChunkLod(node->children[i], lod);
		if (node->children[i])
		{
			OctreeNode *child = node->children[i];
			if (child->type == Node_Internal)
			{
				isCollapsible = false;
			}
			else
			{
				qef.add(child->drawInfo->qef);

				midsign = (child->drawInfo->corners >> (7 - i)) & 1;
				signs[i] = (child->drawInfo->corners >> i) & 1;

				edgeCount++;
			}
		}
	}

	if (!isCollapsible)
	{
		// at least one child is an internal node, can't collapse
		return node;
	}

	svd::Vec3 qefPosition;
	qef.solve(qefPosition, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);
	float error = qef.getError();

	// convert to glm vec3 for ease of use
	vm::vec3 position(qefPosition.x, qefPosition.y, qefPosition.z);

	// at this point the masspoint will actually be a sum, so divide to make it the average
	if (node->size > lod)
	{
		return node;
	}

	if (position.x < node->min.x || position.x > (node->min.x + node->size) ||
		position.y < node->min.y || position.y > (node->min.y + node->size) ||
		position.z < node->min.z || position.z > (node->min.z + node->size))
	{
		const auto &mp = qef.getMassPoint();
		position = vm::vec3(mp.x, mp.y, mp.z);
	}

	// change the node from an internal node to a 'psuedo leaf' node
	OctreeDrawInfo *drawInfo = new OctreeDrawInfo;

	for (int i = 0; i < 8; i++)
	{
		if (signs[i] == -1)
		{
			// Undetermined, use centre sign instead
			drawInfo->corners |= (midsign << i);
		}
		else
		{
			drawInfo->corners |= (signs[i] << i);
		}
	}

	drawInfo->averageNormal = vm::vec3(0.f);

	const int numberOfBiomeSamples = 8 * 4;
	unsigned char biomeSamples[numberOfBiomeSamples];
	unsigned char biomeWeightsSamples[numberOfBiomeSamples];

	for (int i = 0; i < 8; i++)
	{
		if (OctreeNode *child = node->children[i])
		{
			if (child->type == Node_Psuedo ||
				child->type == Node_Leaf)
			{
				drawInfo->averageNormal += child->drawInfo->averageNormal;
				biomeSamples[i] = child->drawInfo->biome.x;
				biomeSamples[i + 1] = child->drawInfo->biome.y;
				biomeSamples[i + 2] = child->drawInfo->biome.z;
				biomeSamples[i + 3] = child->drawInfo->biome.w;

				biomeWeightsSamples[i] = child->drawInfo->biomeWeights.x;
				biomeWeightsSamples[i + 1] = child->drawInfo->biomeWeights.y;
				biomeWeightsSamples[i + 2] = child->drawInfo->biomeWeights.z;
				biomeWeightsSamples[i + 3] = child->drawInfo->biomeWeights.w;
			}
		}
	}

	drawInfo->averageNormal = vm::normalize(drawInfo->averageNormal);
	drawInfo->position = position;
	drawInfo->qef = qef.getData();
	drawInfo->biome = selectFourMostCommonMembers(biomeSamples, numberOfBiomeSamples);
	drawInfo->biomeWeights = selectFourMostCommonMembers(biomeWeightsSamples, numberOfBiomeSamples);

	for (int i = 0; i < 8; i++)
	{
		destroyOctree(node->children[i]);
		node->children[i] = nullptr;
	}

	node->type = Node_Leaf;
	node->drawInfo = drawInfo;

	return node;
}

void cloneNode(OctreeNode *sourceNode, OctreeNode *node)
{
	if (!sourceNode || !node)
	{
		return;
	}

	node->type = sourceNode->type;
	node->min = sourceNode->min;
	node->size = sourceNode->size;

	if (sourceNode->drawInfo)
	{
		node->drawInfo = new OctreeDrawInfo;
		node->drawInfo->position = sourceNode->drawInfo->position;
		node->drawInfo->averageNormal = sourceNode->drawInfo->averageNormal;
		node->drawInfo->corners = sourceNode->drawInfo->corners;
		node->drawInfo->index = sourceNode->drawInfo->index;
		node->drawInfo->qef = sourceNode->drawInfo->qef;
		node->drawInfo->biome = sourceNode->drawInfo->biome;
		node->drawInfo->biomeWeights = sourceNode->drawInfo->biomeWeights;
	}

	for (int i = 0; i < 8; i++)
	{
		if (!sourceNode->children[i])
		{
			continue;
		}
		node->children[i] = new OctreeNode;
		cloneNode(sourceNode->children[i], node->children[i]);
	}
}

OctreeNode *createChunkWithLod(OctreeNode *chunkRoot)
{
	if (!chunkRoot)
	{
		return nullptr;
	}
	// set the active lod of the root
	OctreeNode *chunk = new OctreeNode;
	// clone chunk root data
	cloneNode(chunkRoot, chunk);
	// apply lod to chunk clone
	chunk = switchChunkLod(chunk, chunkRoot->lod);

	return chunk;
}

const vm::ivec3 chunkMinForPosition(const vm::ivec3 &p)
{
	const unsigned int mask = ~(64 - 1);
	return vm::ivec3(p.x & mask, p.y & mask, p.z & mask);
}

uint64_t hashOctreeMin(const vm::ivec3 &min)
{
	uint64_t result = uint16_t(min.x);
	result = (result << 16) + uint16_t(min.y);
	result = (result << 16) + uint16_t(min.z);
	return result;
}

OctreeNode *getChunkRootFromHashMap(vm::ivec3 octreeMin, std::unordered_map<uint64_t, OctreeNode *> &hashMap)
{
	const uint64_t rootIndex = hashOctreeMin(octreeMin);
	auto iter = hashMap.find(rootIndex);
	if (iter == std::end(hashMap))
	{
		// Did not found the octree
		return nullptr;
	}
	else
	{
		// Found the octree
		return iter->second;
	}
}

void addChunkRootToHashMap(OctreeNode *root, std::unordered_map<uint64_t, OctreeNode *> &hashMap)
{
	const uint64_t rootIndex = hashOctreeMin(root->min);
	// std::cout << "Adding something here : " << rootIndex << endl;
	hashMap.insert(std::pair<uint64_t, OctreeNode *>(rootIndex, root));
}

std::vector<OctreeNode *> constructParents(
	OctreeNode *octree,
	const std::vector<OctreeNode *> &nodes,
	const int parentSize,
	const vm::ivec3 &rootMin)
{
	std::unordered_map<uint64_t, OctreeNode *> parentsHashmap;

	for_each(begin(nodes), end(nodes), [&](OctreeNode *node)
			 {
				 // because the octree is regular we can calculate the parent min
				 const vm::ivec3 localPos = (node->min - rootMin);
				 const vm::ivec3 parentPos = node->min - (localPos % parentSize);

				 const uint64_t parentIndex = hashOctreeMin(parentPos - rootMin);
				 OctreeNode *parentNode = nullptr;

				 auto iter = parentsHashmap.find(parentIndex);
				 if (iter == end(parentsHashmap))
				 {
					 parentNode = new OctreeNode;
					 parentNode->type = Node_Internal;
					 parentNode->min = parentPos;
					 parentNode->size = parentSize;

					 parentsHashmap.insert(std::pair<uint64_t, OctreeNode *>(parentIndex, parentNode));
				 }
				 else
				 {
					 parentNode = iter->second;
				 }

				 bool foundParentNode = false;
				 for (int i = 0; i < 8; i++)
				 {
					 const vm::ivec3 childPos = parentPos + ((parentSize / 2) * CHILD_MIN_OFFSETS[i]);
					 if (childPos == node->min)
					 {
						 parentNode->children[i] = node;
						 foundParentNode = true;
						 break;
					 }
				 } });

	std::vector<OctreeNode *> parents;
	for_each(begin(parentsHashmap), end(parentsHashmap), [&](std::pair<uint64_t, OctreeNode *> pair)
			 { parents.push_back(pair.second); });

	return parents;
}

OctreeNode *constructOctreeUpwards(
	OctreeNode *octree,
	const std::vector<OctreeNode *> &inputNodes,
	const vm::ivec3 &rootMin,
	const int rootNodeSize)
{
	if (inputNodes.empty())
	{
		return nullptr;
	}

	std::vector<OctreeNode *> nodes(begin(inputNodes), end(inputNodes));
	std::sort(std::begin(nodes), std::end(nodes),
			  [](OctreeNode *&lhs, OctreeNode *&rhs)
			  {
				  return lhs->size < rhs->size;
			  });

	// the input nodes may be different sizes if a seam octree is being constructed
	// in that case we need to process the input nodes in stages along with the newly
	// constructed parent nodes until all the nodes have the same size
	while (nodes.front()->size != nodes.back()->size)
	{
		// find the end of this run
		auto iter = std::begin(nodes);
		int size = (*iter)->size;
		do
		{
			++iter;
		} while ((*iter)->size == size);

		// construct the new parent nodes for this run
		std::vector<OctreeNode *> newNodes(std::begin(nodes), iter);
		newNodes = constructParents(octree, newNodes, size * 2, rootMin);

		// set up for the next iteration: the parents produced plus any remaining input nodes
		newNodes.insert(std::end(newNodes), iter, std::end(nodes));
		std::swap(nodes, newNodes);
	}

	int parentSize = nodes.front()->size * 2;
	while (parentSize <= rootNodeSize)
	{
		nodes = constructParents(octree, nodes, parentSize, rootMin);
		parentSize *= 2;
	}
	OctreeNode *root = nodes.front();

	return root;
}

void generateVertexIndices(OctreeNode *node, VertexBuffer &vertexBuffer)
{
	if (!node)
	{
		return;
	}

	if (node->type == Node_Internal)
	{
		for (int i = 0; i < 8; i++)
		{
			generateVertexIndices(node->children[i], vertexBuffer);
		}
	}
	else
	{
		OctreeDrawInfo *d = node->drawInfo;
		// std::cout << node->size << endl;
		if (!d)
		{
			printf("Error! Could not add vertex!\n");
			// exit(EXIT_FAILURE);
			return;
		}

		d->index = vertexBuffer.positions.size();
		vertexBuffer.positions.push_back(d->position);
		vertexBuffer.normals.push_back(d->averageNormal);
		vertexBuffer.biomes.push_back(d->biome);
		vertexBuffer.biomesWeights.push_back(d->biomeWeights);
	}
}

void contourProcessEdge(OctreeNode *node[4], int dir, IndexBuffer &indexBuffer)
{
	int minSize = 1000000; // arbitrary big number
	int minIndex = 0;
	int indices[4] = {-1, -1, -1, -1};
	bool flip = false;
	bool signChange[4] = {false, false, false, false};

	for (int i = 0; i < 4; i++)
	{
		const int edge = processEdgeMask[dir][i];
		const int c1 = edgevmap[edge][0];
		const int c2 = edgevmap[edge][1];

		const int m1 = (node[i]->drawInfo->corners >> c1) & 1;
		const int m2 = (node[i]->drawInfo->corners >> c2) & 1;

		if (node[i]->size < minSize)
		{
			minSize = node[i]->size;
			minIndex = i;
			flip = m1 != MATERIAL_AIR;
		}

		indices[i] = node[i]->drawInfo->index;

		signChange[i] =
			(m1 == MATERIAL_AIR && m2 != MATERIAL_AIR) ||
			(m1 != MATERIAL_AIR && m2 == MATERIAL_AIR);
	}

	if (signChange[minIndex])
	{
		if (!flip)
		{
			indexBuffer.push_back(indices[0]);
			indexBuffer.push_back(indices[1]);
			indexBuffer.push_back(indices[3]);

			indexBuffer.push_back(indices[0]);
			indexBuffer.push_back(indices[3]);
			indexBuffer.push_back(indices[2]);
		}
		else
		{
			indexBuffer.push_back(indices[0]);
			indexBuffer.push_back(indices[3]);
			indexBuffer.push_back(indices[1]);

			indexBuffer.push_back(indices[0]);
			indexBuffer.push_back(indices[2]);
			indexBuffer.push_back(indices[3]);
		}
	}
}

void contourEdgeProc(OctreeNode *node[4], int dir, IndexBuffer &indexBuffer, bool isSeam)
{
	if (!node[0] || !node[1] || !node[2] || !node[3])
	{
		return;
	}

	if (isSeam)
	{
		std::vector<vm::ivec3> chunkRoots;
		for (int i = 0; i < 4; i++)
		{
			const uint64_t chunkIndex = hashOctreeMin(chunkMinForPosition(node[i]->min));
			chunkRoots.push_back(chunkMinForPosition(node[i]->min));
		}
		if (chunkRoots.size() == 1)
		{
			return;
		}
	}

	const bool isBranch[4] =
		{
			node[0]->type == Node_Internal,
			node[1]->type == Node_Internal,
			node[2]->type == Node_Internal,
			node[3]->type == Node_Internal,
		};

	if (!isBranch[0] && !isBranch[1] && !isBranch[2] && !isBranch[3])
	{
		contourProcessEdge(node, dir, indexBuffer);
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			OctreeNode *edgeNodes[4];
			const int c[4] =
				{
					edgeProcEdgeMask[dir][i][0],
					edgeProcEdgeMask[dir][i][1],
					edgeProcEdgeMask[dir][i][2],
					edgeProcEdgeMask[dir][i][3],
				};

			for (int j = 0; j < 4; j++)
			{
				if (!isBranch[j])
				{
					edgeNodes[j] = node[j];
				}
				else
				{
					edgeNodes[j] = node[j]->children[c[j]];
				}
			}

			contourEdgeProc(edgeNodes, edgeProcEdgeMask[dir][i][4], indexBuffer, isSeam);
		}
	}
}

void contourFaceProc(OctreeNode *node[2], int dir, IndexBuffer &indexBuffer, bool isSeam)
{
	if (!node[0] || !node[1])
	{
		return;
	}

	// if (isSeam == true)
	// {
	// 	if (chunkMinForPosition(node[0]->min) == chunkMinForPosition(node[1]->min))
	// 	{
	// 		return;
	// 	}
	// }

	const bool isBranch[2] =
		{
			node[0]->type == Node_Internal,
			node[1]->type == Node_Internal,
		};

	if (isBranch[0] || isBranch[1])
	{
		for (int i = 0; i < 4; i++)
		{
			OctreeNode *faceNodes[2];
			const int c[2] =
				{
					faceProcFaceMask[dir][i][0],
					faceProcFaceMask[dir][i][1],
				};

			for (int j = 0; j < 2; j++)
			{
				if (!isBranch[j])
				{
					faceNodes[j] = node[j];
				}
				else
				{
					faceNodes[j] = node[j]->children[c[j]];
				}
			}

			contourFaceProc(faceNodes, faceProcFaceMask[dir][i][2], indexBuffer, isSeam);
		}

		const int orders[2][4] =
			{
				{0, 0, 1, 1},
				{0, 1, 0, 1},
			};
		for (int i = 0; i < 4; i++)
		{
			OctreeNode *edgeNodes[4];
			const int c[4] =
				{
					faceProcEdgeMask[dir][i][1],
					faceProcEdgeMask[dir][i][2],
					faceProcEdgeMask[dir][i][3],
					faceProcEdgeMask[dir][i][4],
				};

			const int *order = orders[faceProcEdgeMask[dir][i][0]];
			for (int j = 0; j < 4; j++)
			{
				if (!isBranch[order[j]])
				{
					edgeNodes[j] = node[order[j]];
				}
				else
				{
					edgeNodes[j] = node[order[j]]->children[c[j]];
				}
			}

			contourEdgeProc(edgeNodes, faceProcEdgeMask[dir][i][5], indexBuffer, isSeam);
		}
	}
}

void contourCellProc(OctreeNode *node, IndexBuffer &indexBuffer, bool isSeam)
{
	if (node == NULL || node->type == Node_Leaf)
	{
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		contourCellProc(node->children[i], indexBuffer, isSeam);
	}

	for (int i = 0; i < 12; i++)
	{
		OctreeNode *faceNodes[2];
		const int c[2] = {cellProcFaceMask[i][0], cellProcFaceMask[i][1]};

		faceNodes[0] = node->children[c[0]];
		faceNodes[1] = node->children[c[1]];

		contourFaceProc(faceNodes, cellProcFaceMask[i][2], indexBuffer, isSeam);
	}

	for (int i = 0; i < 6; i++)
	{
		OctreeNode *edgeNodes[4];
		const int c[4] =
			{
				cellProcEdgeMask[i][0],
				cellProcEdgeMask[i][1],
				cellProcEdgeMask[i][2],
				cellProcEdgeMask[i][3],
			};

		for (int j = 0; j < 4; j++)
		{
			edgeNodes[j] = node->children[c[j]];
		}

		contourEdgeProc(edgeNodes, cellProcEdgeMask[i][4], indexBuffer, isSeam);
	}
}

vm::vec3 approximateZeroCrossingPosition(const vm::vec3 &p0, const vm::vec3 &p1, CachedNoise &chunkNoise)
{
	// approximate the zero crossing by finding the min value along the edge
	float minValue = 100000.f;
	float t = 0.f;
	float currentT = 0.f;
	const int steps = 8;
	const float increment = 1.f / (float)steps;
	while (currentT <= 1.f)
	{
		const vm::vec3 p = p0 + ((p1 - p0) * currentT);
		const float density = abs(Density_Func(p, chunkNoise));
		if (density < minValue)
		{
			minValue = density;
			t = currentT;
		}

		currentT += increment;
	}

	return p0 + ((p1 - p0) * t);
}

vm::vec3 calculateSurfaceNormal(const vm::vec3 &p, CachedNoise &chunkNoise)
{
	const float H = 0.001f;
	const float dx = Density_Func(p + vm::vec3(H, 0.f, 0.f), chunkNoise) - Density_Func(p - vm::vec3(H, 0.f, 0.f), chunkNoise);
	const float dy = Density_Func(p + vm::vec3(0.f, H, 0.f), chunkNoise) - Density_Func(p - vm::vec3(0.f, H, 0.f), chunkNoise);
	const float dz = Density_Func(p + vm::vec3(0.f, 0.f, H), chunkNoise) - Density_Func(p - vm::vec3(0.f, 0.f, H), chunkNoise);

	return vm::normalize(vm::vec3(dx, dy, dz));
}

OctreeNode *constructLeaf(OctreeNode *leaf, CachedNoise &chunkNoise)
{
	if (!leaf)
	{
		return nullptr;
	}

	int corners = 0;
	for (int i = 0; i < 8; i++)
	{
		const vm::ivec3 cornerPos = leaf->min + CHILD_MIN_OFFSETS[i];
		const float density = Density_Func(vm::vec3(cornerPos.x, cornerPos.y, cornerPos.z), chunkNoise);
		const int material = density < 0.f ? MATERIAL_SOLID : MATERIAL_AIR;
		corners |= (material << i);
	}

	if (corners == 0 || corners == 255)
	{
		// voxel is full inside or outside the volume
		delete leaf;
		return nullptr;
	}

	// otherwise the voxel contains the surface, so find the edge intersections
	const int MAX_CROSSINGS = 6;
	int edgeCount = 0;
	vm::vec3 averageNormal(0.f);
	svd::QefSolver qef;

	for (int i = 0; i < 12 && edgeCount < MAX_CROSSINGS; i++)
	{
		const int c1 = edgevmap[i][0];
		const int c2 = edgevmap[i][1];

		const int m1 = (corners >> c1) & 1;
		const int m2 = (corners >> c2) & 1;

		if ((m1 == MATERIAL_AIR && m2 == MATERIAL_AIR) ||
			(m1 == MATERIAL_SOLID && m2 == MATERIAL_SOLID))
		{
			// no zero crossing on this edge
			continue;
		}

		const vm::vec3 p1 = vm::vec3(leaf->min.x + CHILD_MIN_OFFSETS[c1].x, leaf->min.y + CHILD_MIN_OFFSETS[c1].y, leaf->min.z + CHILD_MIN_OFFSETS[c1].z);
		const vm::vec3 p2 = vm::vec3(leaf->min.x + CHILD_MIN_OFFSETS[c2].x, leaf->min.y + CHILD_MIN_OFFSETS[c2].y, leaf->min.z + CHILD_MIN_OFFSETS[c2].z);
		const vm::vec3 p = approximateZeroCrossingPosition(p1, p2, chunkNoise);
		const vm::vec3 n = calculateSurfaceNormal(p, chunkNoise);
		qef.add(p.x, p.y, p.z, n.x, n.y, n.z);

		averageNormal += n;

		edgeCount++;
	}

	svd::Vec3 qefPosition;
	qef.solve(qefPosition, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);

	OctreeDrawInfo *drawInfo = new OctreeDrawInfo;
	drawInfo->position = vm::vec3(qefPosition.x, qefPosition.y, qefPosition.z);
	drawInfo->qef = qef.getData();

	const vm::vec3 min = vm::vec3(leaf->min.x, leaf->min.y, leaf->min.z);
	const vm::vec3 max = vm::vec3(leaf->min.x + vm::ivec3(leaf->size).x, leaf->min.y + vm::ivec3(leaf->size).y, leaf->min.z + vm::ivec3(leaf->size).z);
	if (drawInfo->position.x < min.x || drawInfo->position.x > max.x ||
		drawInfo->position.y < min.y || drawInfo->position.y > max.y ||
		drawInfo->position.z < min.z || drawInfo->position.z > max.z)
	{
		const auto &mp = qef.getMassPoint();
		drawInfo->position = vm::vec3(mp.x, mp.y, mp.z);
	}

	drawInfo->averageNormal = normalize(averageNormal / (float)edgeCount);
	drawInfo->corners = corners;

	drawInfo->biome = getBiomesData(drawInfo->position, chunkNoise);
	drawInfo->biomeWeights = getBiomesWeight(drawInfo->position, drawInfo->biome, chunkNoise);

	leaf->type = Node_Leaf;
	leaf->drawInfo = drawInfo;

	return leaf;
}

typedef std::function<bool(const vm::ivec3 &, const vm::ivec3 &)> FilterNodesFunc;

void findOctreeNodes(OctreeNode *node, FilterNodesFunc &func, std::vector<OctreeNode *> &nodes)
{
	if (!node)
	{
		return;
	}

	const vm::ivec3 max = node->min + vm::ivec3(node->size);
	if (!func(node->min, max))
	{
		return;
	}

	if (node->type == Node_Leaf)
	{
		nodes.push_back(node);
	}
	else
	{
		for (int i = 0; i < 8; i++)
			findOctreeNodes(node->children[i], func, nodes);
	}
}

std::vector<OctreeNode *> findNodes(OctreeNode *root, FilterNodesFunc filterFunc)
{
	std::vector<OctreeNode *> nodes;
	findOctreeNodes(root, filterFunc, nodes);
	return nodes;
}
std::vector<OctreeNode *> findSeamNodes(OctreeNode *targetRoot, std::vector<OctreeNode *>(&neighbouringChunks), std::unordered_map<uint64_t, OctreeNode *> hashMap, OctreeNode *(*getChunkRootFromHashMap)(vm::ivec3, std::unordered_map<uint64_t, OctreeNode *> &))
{
	const vm::ivec3 baseChunkMin = vm::ivec3(targetRoot->min);
	const vm::ivec3 seamValues = baseChunkMin + vm::ivec3(targetRoot->size);

	const vm::ivec3 OFFSETS[8] =
		{
			vm::ivec3(0, 0, 0), vm::ivec3(1, 0, 0), vm::ivec3(0, 0, 1), vm::ivec3(1, 0, 1),
			vm::ivec3(0, 1, 0), vm::ivec3(1, 1, 0), vm::ivec3(0, 1, 1), vm::ivec3(1, 1, 1)};

	FilterNodesFunc selectionFuncs[8] =
		{
			[&](const vm::ivec3 &min, const vm::ivec3 &max)
			{
				return max.x == seamValues.x || max.y == seamValues.y || max.z == seamValues.z;
			},
			[&](const vm::ivec3 &min, const vm::ivec3 &max)
			{
				return min.x == seamValues.x;
			},
			[&](const vm::ivec3 &min, const vm::ivec3 &max)
			{
				return min.z == seamValues.z;
			},
			[&](const vm::ivec3 &min, const vm::ivec3 &max)
			{
				return min.x == seamValues.x && min.z == seamValues.z;
			},
			[&](const vm::ivec3 &min, const vm::ivec3 &max)
			{
				return min.y == seamValues.y;
			},
			[&](const vm::ivec3 &min, const vm::ivec3 &max)
			{
				return min.x == seamValues.x && min.y == seamValues.y;
			},
			[&](const vm::ivec3 &min, const vm::ivec3 &max)
			{
				return min.y == seamValues.y && min.z == seamValues.z;
			},
			[&](const vm::ivec3 &min, const vm::ivec3 &max)
			{
				return min.x == seamValues.x && min.y == seamValues.y && min.z == seamValues.z;
			}};

	std::vector<OctreeNode *> seamNodes;
	for (int i = 0; i < 8; i++)
	{
		const vm::ivec3 offsetMin = OFFSETS[i] * targetRoot->size;
		const vm::ivec3 chunkMin = baseChunkMin + offsetMin;
		if (OctreeNode *chunkRoot = getChunkRootFromHashMap(chunkMin, hashMap))
		{
			if (OctreeNode *chunkWithLod = createChunkWithLod(chunkRoot))
			{
				neighbouringChunks.push_back(chunkWithLod);
				std::vector<OctreeNode *> chunkNodes = findNodes(chunkWithLod, selectionFuncs[i]);
				seamNodes.insert(std::end(seamNodes), std::begin(chunkNodes), std::end(chunkNodes));
			}
		}
	}

	return seamNodes;
}

OctreeNode *constructOctreeNodes(OctreeNode *node, CachedNoise &chunkNoise)
{
	if (!node)
	{
		return nullptr;
	}

	if (node->size == 1)
	{
		return constructLeaf(node, chunkNoise);
	}

	const int childSize = node->size / 2;
	bool hasChildren = false;

	for (int i = 0; i < 8; i++)
	{
		OctreeNode *child = new OctreeNode;
		child->size = childSize;
		child->min = node->min + (CHILD_MIN_OFFSETS[i] * childSize);
		child->type = Node_Internal;

		node->children[i] = constructOctreeNodes(child, chunkNoise);
		hasChildren |= (node->children[i] != nullptr);
	}

	if (!hasChildren)
	{
		delete node;
		return nullptr;
	}

	return node;
}

OctreeNode *constructOctreeDownwards(const vm::ivec3 &min, const int size, CachedNoise &chunkNoise)
{
	OctreeNode *root = new OctreeNode;
	root->min = min;
	root->size = size;
	root->type = Node_Internal;

	const OctreeNode *octreeRootNode = constructOctreeNodes(root, chunkNoise);
	if (!octreeRootNode)
	{
		return nullptr;
	}

	return root;
}

void generateMeshFromOctree(OctreeNode *node, bool isSeam, VertexBuffer &vertexBuffer)
{
	if (!node)
	{
		return;
	}

	generateVertexIndices(node, vertexBuffer);
	contourCellProc(node, vertexBuffer.indices, isSeam);
}

void destroyOctree(OctreeNode *node)
{
	if (!node)
	{
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		destroyOctree(node->children[i]);
	}

	if (node->drawInfo)
	{
		delete node->drawInfo;
	}

	delete node;
}

void addNodesToVector(OctreeNode *node, std::vector<OctreeNode *> &vector)
{
	if (!node)
	{
		return;
	}
	for (int i = 0; i < 8; i++)
	{
		addNodesToVector(node->children[i], vector);
	}
	vector.push_back(node);
}