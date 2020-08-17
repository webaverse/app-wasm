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
};
class GeometrySet {
public:
	std::vector<Geometry> geometries;
};

GeometrySet *doMakeGeometrySet() {
	return new GeometrySet();
}
void doLoadBake(GeometrySet *geometrySet, unsigned char *data, unsigned int size) {
  Geometry geometry;

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
	  geometry.positions.resize(numValues);
	  memcpy(geometry.positions.data(), src, numValues*sizeof(float));
	}
	{
	  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::TEX_COORD);
	  int numComponents = attribute->num_components();
	  int numPoints = mesh->num_points();
	  int numValues = numPoints * numComponents;

	  float *src = (float *)attribute->buffer()->data();
	  geometry.uvs.resize(numValues);
	  memcpy(geometry.uvs.data(), src, numValues*sizeof(float));
	}
	{
	  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::COLOR);
	  int numComponents = attribute->num_components();
	  int numPoints = mesh->num_points();
	  int numValues = numPoints * numComponents;

	  float *src = (float *)attribute->buffer()->data();
	  geometry.colors.resize(numValues);
	  memcpy(geometry.colors.data(), src, numValues*sizeof(float));
	}
	{
    int numFaces = mesh->num_faces();
    int numIndices = numFaces * 3;

    geometry.indices.resize(numIndices);
    for (draco::FaceIndex i(0); i < numFaces; i++) {
    	const draco::Mesh::Face &face = mesh->face(i);
    	unsigned int *indicesSrc = (unsigned int *)face.data();
    	memcpy(&geometry.indices[i.value()*3], indicesSrc, 3*sizeof(unsigned int));
    }
  }

  geometrySet->geometries.push_back(std::move(geometry));
}