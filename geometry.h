#include "vector.h"
#include "draco/compression/decode.h"

class Geometry {
public:
  std::string name;
  bool transparent;
  bool vegetation;
  bool animal;
  std::vector<float> positions;
  std::vector<float> uvs;
  std::vector<float> colors;
  std::vector<unsigned int> indices;
  std::vector<float> heads;
  std::vector<float> legs;
  Vec headPivot;
  Box aabb;
};
class GeometrySet {
public:
	std::vector<Geometry *> geometries;
	std::vector<Geometry *> animalGeometries;
	std::map<std::string, Geometry *> geometryMap;
};

GeometrySet *doMakeGeometrySet() {
	return new GeometrySet();
}
void doLoadBake(GeometrySet *geometrySet, unsigned char *data, unsigned int size) {
  Geometry *geometry = new Geometry();

  draco::Decoder decoder;

  draco::DecoderBuffer buffer;
  buffer.Init((char *)data, size);

	std::unique_ptr<draco::Mesh> mesh = decoder.DecodeMeshFromBuffer(&buffer).value();

	const draco::GeometryMetadata *metadata = mesh->GetMetadata();
	std::string name;
	metadata->GetEntryString("name", &name);
	std::string transparent;
	metadata->GetEntryString("transparent", &transparent);
	std::string vegetation;
	metadata->GetEntryString("vegetation", &vegetation);
	std::string animal;
	metadata->GetEntryString("animal", &animal);

  {
	  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::POSITION);
	  int numComponents = attribute->num_components();
	  int numPoints = mesh->num_points();
	  int numValues = numPoints * numComponents;

	  float *src = (float *)attribute->buffer()->data();
	  geometry->positions.resize(numValues);
	  memcpy(geometry->positions.data(), src, numValues*sizeof(float));
	}
	{
	  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::TEX_COORD);
	  int numComponents = attribute->num_components();
	  int numPoints = mesh->num_points();
	  int numValues = numPoints * numComponents;

	  float *src = (float *)attribute->buffer()->data();
	  geometry->uvs.resize(numValues);
	  memcpy(geometry->uvs.data(), src, numValues*sizeof(float));
	}
	{
	  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::COLOR);
	  int numComponents = attribute->num_components();
	  int numPoints = mesh->num_points();
	  int numValues = numPoints * numComponents;

	  float *src = (float *)attribute->buffer()->data();
	  geometry->colors.resize(numValues);
	  memcpy(geometry->colors.data(), src, numValues*sizeof(float));
	}
	{
    int numFaces = mesh->num_faces();
    int numIndices = numFaces * 3;

    geometry->indices.resize(numIndices);
    for (draco::FaceIndex i(0); i < numFaces; i++) {
    	const draco::Mesh::Face &face = mesh->face(i);
    	unsigned int *indicesSrc = (unsigned int *)face.data();
    	memcpy(&geometry->indices[i.value()*3], indicesSrc, 3*sizeof(unsigned int));
    }
  }

  geometrySet->geometries.push_back(geometry);
  if (animal == "1") {
    geometry->aabb.setFromPositions(geometry->positions.data(), geometry->positions.size());
    Vec center = geometry->aabb.center();
    Vec size = geometry->aabb.size();
    geometry->headPivot = center - (size * Vec(0.0f, 1.0f/2.0f * 0.5f, -1.0f/2.0f * 0.5f));
    Vec legsPivot = center - (size * Vec(0.0f, -1.0f/2.0f + 1.0f/3.0f, 0.0f));

	  constexpr float legsSepFactor = 0.5;
	  Vec legsPivotTopLeft = legsPivot + (size * Vec(-1.0f/2.0f * legsSepFactor, 0.0f, -1.0f/2.0f * legsSepFactor));
    Vec legsPivotTopRight = legsPivot + (size * Vec(1.0f/2.0f * legsSepFactor, 0.0f, -1.0f/2.0f * legsSepFactor));
    Vec legsPivotBottomLeft = legsPivot + (size * Vec(-1.0f/2.0f * legsSepFactor, 0.0f, 1.0f/2.0f * legsSepFactor));
    Vec legsPivotBottomRight = legsPivot + (size * Vec(1.0f/2.0f * legsSepFactor, 0.0f, 1.0f/2.0f * legsSepFactor));

    geometry->heads.resize(geometry->positions.size());
    geometry->legs.resize(geometry->legs.size()/3*4);
	  for (unsigned int i = 0, j = 0; i < geometry->positions.size(); i += 3, j += 4) {
	  	Vec head{
        geometry->positions[i],
        geometry->positions[i+1],
        geometry->positions[i+2],
	  	};
	    if (head.z < geometry->headPivot.z) {
	    	head -= geometry->headPivot;
	    } else {
	    	head = Vec();
	    }
	    geometry->heads[i] = head.x;
	    geometry->heads[i+1] = head.y;
	    geometry->heads[i+2] = head.z;

      Vec position{
        geometry->positions[i],
        geometry->positions[i+1],
        geometry->positions[i+2],
	  	};
	    float xAxis;
	    if (position.y < legsPivot.y) {
	      if (position.x >= legsPivot.x) {
	        if (position.z >= legsPivot.z) {
	          position -= legsPivotBottomRight;
	          xAxis = 1;
	        } else {
	          position -= legsPivotTopRight;
	          xAxis = -1;
	        }
	      } else {
	        if (position.z >= legsPivot.z) {
	          position -= legsPivotBottomLeft;
	          xAxis = -1;
	        } else {
	          position -= legsPivotTopLeft;
	          xAxis = 1;
	        }
	      }
	    } else {
	    	position = Vec();
	      xAxis = 0;
	    }
	    geometry->legs[j] = position.x;
	    geometry->legs[j+1] = position.y;
	    geometry->legs[j+2] = position.z;
	    geometry->legs[j+3] = xAxis;
	  }

    geometrySet->animalGeometries.push_back(geometry);
  }
  geometrySet->geometryMap[name] = geometry;
}

void getAnimalGeometry(GeometrySet *geometrySet, unsigned int hash, float **positions, float **colors, float **heads, float **legs, unsigned int **indices, unsigned int &numPositions, unsigned int &numColors, unsigned int &numIndices, unsigned int &numHeads, unsigned int &numLegs, float *headPivot, float *aabb) {
  unsigned int animalGeometryIndex = hash/0xFFFFFF*geometrySet->animalGeometries.size();
  Geometry *geometry = geometrySet->animalGeometries[animalGeometryIndex];

  *positions = geometry->positions.data();
  *colors = geometry->colors.data();
  *indices = geometry->indices.data();
  *heads = geometry->heads.data();
  *legs = geometry->legs.data();

  numPositions = geometry->positions.size();
  numColors = geometry->colors.size();
  numIndices = geometry->indices.size();
  numHeads = geometry->heads.size();
  numLegs = geometry->legs.size();

  memcpy(headPivot, &geometry->headPivot, sizeof(geometry->headPivot));
  memcpy(aabb, &geometry->aabb, sizeof(geometry->aabb));
}