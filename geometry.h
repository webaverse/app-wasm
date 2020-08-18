#include "vector.h"
#include "draco/compression/decode.h"
#include "draco/compression/encode.h"
// #include "draco/mesh/triangle_soup_mesh_builder.h"
// #include "draco/javascript/emscripten/encoder_webidl_wrapper.h"

inline unsigned int align4(unsigned int n) {
  unsigned int d = n%4;
  return d ? (n+4-d) : n;
}

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
  unsigned int index = 0;
  while (index < size) {
    unsigned int geometrySize = *((unsigned int *)(data + index));
	  index += sizeof(unsigned int);
	  unsigned char *geometryData = data + index;
	  index += geometrySize;
	  index = align4(index);

    Geometry *geometry = new Geometry();

	  draco::Decoder decoder;

	  draco::DecoderBuffer buffer;
	  buffer.Init((char *)geometryData, geometrySize);

    auto sor = decoder.DecodeMeshFromBuffer(&buffer);
		auto &mesh = sor.value();
		if (!mesh) {
			auto &status = sor.status();
			std::cout << "status code " << status.code() << " " << status.error_msg_string() << std::endl;
		}

		std::cout << "decode mesh " << (void *)mesh.get() << std::endl;

		const draco::GeometryMetadata *metadata = mesh->GetMetadata();
		std::string name;
		bool ok = metadata->GetEntryString("name", &name);
		std::cout << "geo name " << (int)ok << " " << name << std::endl;
		int transparent;
		metadata->GetEntryInt("transparent", &transparent);
	  int vegetation;
		metadata->GetEntryInt("vegetation", &vegetation);
		int animal;
		metadata->GetEntryInt("animal", &animal);

	  int numPositions;
	  {
		  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::POSITION);
		  int numComponents = attribute->num_components();
		  int numPoints = mesh->num_points();
		  int numValues = numPoints * numComponents;
		  numPositions = numValues;

      geometry->positions.resize(numValues);
      /* for (int i = 0; i < numPoints; i++) {
        attribute->GetValue(draco::AttributeValueIndex(i), &geometry->positions[i*3]);
      } */
		  float *src = (float *)attribute->buffer()->data();
		  geometry->positions.resize(numValues);
		  memcpy(geometry->positions.data(), src, numValues*sizeof(float));
		}
		{
		  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::TEX_COORD);
		  if (attribute) {
			  int numComponents = attribute->num_components();
			  int numPoints = mesh->num_points();
			  int numValues = numPoints * numComponents;

			  float *src = (float *)attribute->buffer()->data();
			  geometry->uvs.resize(numValues);
			  memcpy(geometry->uvs.data(), src, numValues*sizeof(float));
			} else {
				geometry->uvs.resize(numPositions/3*2);
			}
		}
		{
		  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::COLOR);
		  if (attribute) {
			  int numComponents = attribute->num_components();
			  int numPoints = mesh->num_points();
			  int numValues = numPoints * numComponents;

			  float *src = (float *)attribute->buffer()->data();
			  geometry->colors.resize(numValues);
			  memcpy(geometry->colors.data(), src, numValues*sizeof(float));
			} else {
				geometry->colors.resize(numPositions);
			}
		}
		{
	    int numFaces = mesh->num_faces();
	    int numIndices = numFaces * 3;

	    geometry->indices.resize(numIndices);

	    for (draco::FaceIndex i(0); i < numFaces; i++) {
	    	const draco::Mesh::Face &face = mesh->face(i);
	    	/* geometry->indices[i.value()*3] = face[0].value();
	    	geometry->indices[i.value()*3+1] = face[1].value();
	    	geometry->indices[i.value()*3+2] = face[2].value(); */
	    	unsigned int *indicesSrc = (unsigned int *)face.data();
	    	// std::cout << "get face " << i.value() << " " << (void *)indicesSrc << " " << sizeof(face.data()[0]) << std::endl;
	    	memcpy(&geometry->indices[i.value()*3], indicesSrc, 3*sizeof(unsigned int));
	    }
	  }

	  geometrySet->geometries.push_back(geometry);
	  if (animal) {
	    /* geometry->aabb.setFromPositions(geometry->positions.data(), geometry->positions.size());
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
	    geometry->legs.resize(geometry->positions.size()/3*4);
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
		  } */

	    geometrySet->animalGeometries.push_back(geometry);
	  }
	  geometrySet->geometryMap[name] = geometry;
  }
}

void doGetGeometry(GeometrySet *geometrySet, char *nameData, unsigned int nameSize, float **positions, float **uvs, unsigned int **indices, unsigned int &numPositions, unsigned int &numUvs, unsigned int &numIndices) {
  std::string name(nameData, nameSize);
  Geometry *geometry = geometrySet->geometryMap[name];

  *positions = geometry->positions.data();
  *uvs = geometry->uvs.data();
  *indices = geometry->indices.data();

  numPositions = geometry->positions.size();
  numUvs = geometry->uvs.size();
  numIndices = geometry->indices.size();
}

void doGetAnimalGeometry(GeometrySet *geometrySet, unsigned int hash, float **positions, float **colors, float **heads, float **legs, unsigned int **indices, unsigned int &numPositions, unsigned int &numColors, unsigned int &numIndices, unsigned int &numHeads, unsigned int &numLegs, float *headPivot, float *aabb) {
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

constexpr int SUBPARCEL_SIZE = 10;
constexpr int SUBPARCEL_SIZE_P1 = SUBPARCEL_SIZE + 1;
constexpr int MAX_NAME_LENGTH = 32;
int abs(int n) {
  return std::abs(n);
}
int sign(int n) {
  return n < 0 ? 1 : 0;
}
int getSubparcelIndex(int x, int y, int z) {
	return abs(x)|(abs(y)<<9)|(abs(z)<<18)|(sign(x)<<27)|(sign(y)<<28)|(sign(z)<<29);
}
int getFieldIndex(int x, int y, int z) {
	return x + (z * SUBPARCEL_SIZE_P1) + (y * SUBPARCEL_SIZE_P1 * SUBPARCEL_SIZE_P1);
}

class MarchObject {
public:
	unsigned int id;
	char name[MAX_NAME_LENGTH];
	Vec position;
	Quat quaternion;
};
class MarchSpec {
public:
	int index;
  char heightfield[SUBPARCEL_SIZE_P1*SUBPARCEL_SIZE_P1*SUBPARCEL_SIZE_P1 + 1]; // align
  unsigned char lightfield[SUBPARCEL_SIZE_P1*SUBPARCEL_SIZE_P1*SUBPARCEL_SIZE_P1 + 1]; // align
};
void doMarchObjects(GeometrySet *geometrySet, int x, int y, int z, MarchObject *marchObjects, unsigned int numMarchObjects, MarchSpec *marchSpecs, unsigned int numMarchSpecs, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int &numPositions, unsigned int &numUvs, unsigned int &numIds, unsigned int &numIndices) {
  unsigned int &positionsIndex = numPositions;
  unsigned int &uvsIndex = numUvs;
  unsigned int &idsIndex = numIds;
  unsigned int &indicesIndex = numIndices;

  positionsIndex = 0;
  uvsIndex = 0;
  idsIndex = 0;
  indicesIndex = 0;
  unsigned int skyLightsIndex = 0;
  unsigned int torchLightsIndex = 0;

  for (unsigned int i = 0; i < numMarchObjects; i++) {
    MarchObject &marchObject = marchObjects[i];
    std::string name(marchObject.name);
    Geometry *geometry = geometrySet->geometryMap[name];
    Matrix matrix;
    matrix.compose(marchObject.position, marchObject.quaternion, Vec{1, 1, 1});

    unsigned int indexOffset2 = positionsIndex/3;
    for (unsigned int j = 0; j < geometry->indices.size(); j++) {
      indices[indicesIndex + j] = geometry->indices[j] + indexOffset2;
    }
    indicesIndex += geometry->indices.size();

    for (unsigned int j = 0, jOffset = 0; j < geometry->positions.size(); j += 3, jOffset++) {
    	Vec position{
    		geometry->positions[j],
    		geometry->positions[j+1],
    		geometry->positions[j+2],
    	};
    	position.applyMatrix(matrix);
    	positions[positionsIndex + j] = position.x;
  		positions[positionsIndex + j + 1] = position.y;
  		positions[positionsIndex + j + 2] = position.z;

      int ax = (int)std::floor(position.x);
      int ay = (int)std::floor(position.y);
      int az = (int)std::floor(position.z);
      int sx = ax/SUBPARCEL_SIZE;
      int sy = ay/SUBPARCEL_SIZE;
      int sz = az/SUBPARCEL_SIZE;
      int subparcelIndex = getSubparcelIndex(sx, sy, sz);
      MarchSpec *marchSpec = std::find_if(marchSpecs, marchSpecs + numMarchSpecs, [&](const MarchSpec &marchSpec) -> bool {
      	return marchSpec.index == subparcelIndex;
      });
      if (marchSpec) {
        int lx = ax - SUBPARCEL_SIZE*sx;
        int ly = ay - SUBPARCEL_SIZE*sy;
        int lz = az - SUBPARCEL_SIZE*sz;
        int fieldIndex = getFieldIndex(lx, ly, lz);
        skyLights[skyLightsIndex + jOffset] = marchSpec->heightfield[fieldIndex] < 0 ? 0 : marchSpec->heightfield[fieldIndex];
        torchLights[torchLightsIndex + jOffset] = marchSpec->lightfield[fieldIndex];
      } else {
        skyLights[skyLightsIndex + jOffset] = 0;
        torchLights[torchLightsIndex + jOffset] = 0;
      }
    }
    positionsIndex += geometry->positions.size();
    skyLightsIndex += geometry->positions.size()/3;
    torchLightsIndex += geometry->positions.size()/3;

    memcpy(uvs, geometry->uvs.data(), geometry->uvs.size());
    uvsIndex += geometry->uvs.size();

    std::fill(ids + idsIndex, ids + idsIndex + geometry->positions.size()/3, marchObject.id);
    idsIndex += geometry->positions.size()/3;
  }
}

void doEncodeGeometry(float *positions, float *uvs, float *colors, unsigned int *indices, unsigned int numPositions, unsigned int numUvs, unsigned int numColors, unsigned int numIndices, char *nameData, unsigned int nameSize, unsigned int transparent, unsigned int vegetation, unsigned int animal, unsigned char *data, unsigned int &size) {
  std::cout << "encode size 1" << numPositions << " " << numUvs << " " << numColors << " " << numIndices << std::endl;

  draco::Mesh mesh;
  mesh.SetNumFaces(numIndices/3);
  mesh.set_num_points(numIndices);
  for (draco::FaceIndex f(0); f < numIndices/3; ++f) {
  	draco::Mesh::Face face;
  	face[0] = 0;
  	face[1] = 1;
  	face[2] = 2;
  	mesh.SetFace(f, face);
  }

  draco::GeometryAttribute positionsAttribute;
  positionsAttribute.Init(draco::GeometryAttribute::POSITION, nullptr, 3, draco::DT_FLOAT32, false, 12, 0);
  std::unique_ptr<draco::PointAttribute> positions2 = mesh.CreateAttribute(positionsAttribute, true, numIndices/3);
  // positions2->Reset(numPositions/3);
  // memcpy(positions2->buffer()->data(), positions, numPositions*sizeof(float));
  mesh.AddAttribute(std::move(positions2));

  /* draco::GeometryAttribute uvsAttribute;
  uvsAttribute.Init(draco::GeometryAttribute::TEX_COORD, nullptr, 2, draco::DT_FLOAT32, false, 8, 0);
  std::unique_ptr<draco::PointAttribute> uvs2 = mesh.CreateAttribute(uvsAttribute, true, numUvs/2);
  uvs2->Reset(numUvs/2);
  memcpy(uvs2->buffer()->data(), uvs, numUvs*sizeof(float));
  mesh.AddAttribute(std::move(uvs2));

  draco::GeometryAttribute colorsAttribute;
  colorsAttribute.Init(draco::GeometryAttribute::COLOR, nullptr, 3, draco::DT_FLOAT32, false, 12, 0);
  std::unique_ptr<draco::PointAttribute> colors2 = mesh.CreateAttribute(colorsAttribute, true, numColors/3);
  colors2->Reset(numColors/3);
  memcpy(colors2->buffer()->data(), colors, numColors*sizeof(float));
  mesh.AddAttribute(std::move(colors2)); */

  /* std::unique_ptr<draco::GeometryMetadata> metadata = std::unique_ptr<draco::GeometryMetadata>(new draco::GeometryMetadata());
  std::string name(nameData, nameSize);
  metadata->AddEntryString("name", name);
  std::cout << "encode name " << name << std::endl;
  metadata->AddEntryInt("transparent", transparent);
  metadata->AddEntryInt("vegetation", vegetation);
  metadata->AddEntryInt("animal", animal);
  mesh.AddMetadata(std::move(metadata)); */

  // mesh.DeduplicateAttributeValues();
  // mesh.DeduplicatePointIds();

  draco::Encoder encoder;
  encoder.SetEncodingMethod(draco::MESH_SEQUENTIAL_ENCODING);
  /* encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION, 11);
  encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL, 7);
  encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD, 10);
  encoder.SetAttributeQuantization(draco::GeometryAttribute::COLOR, 8); */
  draco::EncoderBuffer buffer;
  auto status = encoder.EncodeMeshToBuffer(mesh, &buffer);
  std::cout << "status code encode " << status.code() << " " << status.error_msg_string() << std::endl;

  // memcpy(data, buffer.data(), buffer.size());
  size = buffer.size();

  std::cout << "encode size 2" << (void *)data << " " << size << " " << numPositions << " " << numUvs << " " << numColors << " " << numIndices << std::endl;

  {
  	draco::Decoder decoder;

	  draco::DecoderBuffer buffer2;
	  buffer2.Init((char *)buffer.data(), buffer.size());

    auto sor = decoder.DecodeMeshFromBuffer(&buffer2);
		auto &mesh = sor.value();
		if (!mesh) {
			auto &status = sor.status();
			std::cout << "status code decode " << status.code() << " " << status.error_msg_string() << std::endl;
		}

		std::cout << "decode mesh " << (void *)mesh.get() << std::endl;
  }

  /* MeshBuilder meshBuilder;
  MetadataBuilder metadataBuilder;

  draco::Mesh dracoMesh;
  draco::Metadata metadata;
  
  metadataBuilder.AddStringEntry(&metadata, "name", nameData);
  metadataBuilder.AddIntEntry(&metadata, "transparent", transparent);
  metadataBuilder.AddIntEntry(&metadata, "vegetation", vegetation);
  metadataBuilder.AddIntEntry(&metadata, "animal", animal);
  meshBuilder.AddMetadata(&dracoMesh, &metadata);

  unsigned int numFaces = numIndices / 3;
  unsigned int numPoints = numPositions;
  meshBuilder.AddFacesToMesh(&dracoMesh, numFaces, (int *)indices);

  meshBuilder.AddFloatAttributeToMesh(&dracoMesh, draco::GeometryAttribute::POSITION, numPoints, 3, positions);
  if (uvs) {
    meshBuilder.AddFloatAttributeToMesh(&dracoMesh, draco::GeometryAttribute::TEX_COORD, numPoints, 2, uvs);
  }
  if (colors) {
    meshBuilder.AddFloatAttributeToMesh(&dracoMesh, draco::GeometryAttribute::COLOR, numPoints, 3, colors);
  }

  Encoder encoder;
  encoder.SetEncodingMethod(draco::MESH_EDGEBREAKER_ENCODING);
  DracoInt8Array encodedData;
  int encodedLen = encoder.EncodeMeshToDracoBuffer(&dracoMesh, &encodedData);
  for (int i = 0; i < encodedLen; i++) {
    data[i] = encodedData.GetValue(i);
  }
  size = encodedLen;

  std::cout << "write geo size " << size << std::endl; */

  /* const encoder = new encoderModule.Encoder();
  const meshBuilder = new encoderModule.MeshBuilder();
  const metadataBuilder = new encoderModule.MetadataBuilder();

  for (const mesh of meshes) {
    let byteArray;
    {
      const dracoMesh = new encoderModule.Mesh();
      const metadata = new encoderModule.Metadata();
      
      metadataBuilder.AddStringEntry(metadata, 'name', mesh.name);
      metadataBuilder.AddIntEntry(metadata, 'transparent', +mesh.transparent);
      metadataBuilder.AddIntEntry(metadata, 'vegetation', +mesh.vegetation);
      metadataBuilder.AddIntEntry(metadata, 'animal', +mesh.animal);
      meshBuilder.AddMetadata(dracoMesh, metadata);

      const numFaces = mesh.indices.length / 3;
      const numPoints = mesh.positions.length;
      meshBuilder.AddFacesToMesh(dracoMesh, numFaces, mesh.indices);

      meshBuilder.AddFloatAttributeToMesh(dracoMesh, encoderModule.POSITION, numPoints, 3, mesh.positions);
      mesh.uvs && meshBuilder.AddFloatAttributeToMesh(dracoMesh, encoderModule.TEX_COORD, numPoints, 2, mesh.uvs);
      mesh.colors && meshBuilder.AddFloatAttributeToMesh(dracoMesh, encoderModule.COLOR, numPoints, 3, mesh.colors);
      encoder.SetEncodingMethod(encoderModule.MESH_EDGEBREAKER_ENCODING);
      // encoder.SetEncodingMethod(encoderModule.MESH_SEQUENTIAL_ENCODING);

      const encodedData = new encoderModule.DracoInt8Array();
      // Use default encoding setting.
      const encodedLen = encoder.EncodeMeshToDracoBuffer(dracoMesh,
                                                         encodedData);
      byteArray = new Uint8Array(encodedLen);
      for (let i = 0; i < encodedLen; i++) {
        byteArray[i] = encodedData.GetValue(i);
      }
      buffers.push(byteArray);
    }
  } */
}