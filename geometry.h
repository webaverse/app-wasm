#include "vector.h"
#include "subparcel.h"
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
  std::vector<unsigned char> colors;
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
bool GetAttributeFloatForAllPoints(const draco::PointCloud &pc,
                                            const draco::PointAttribute &pa,
                                            std::vector<float> &out_values) {
  const int components = pa.num_components();
  const int num_points = pc.num_points();
  const int num_entries = num_points * components;
  const int kMaxAttributeFloatValues = 4;
  float values[kMaxAttributeFloatValues] = {-2.0, -2.0, -2.0, -2.0};
  int entry_id = 0;

  out_values.resize(num_entries);
  for (draco::PointIndex i(0); i < num_points; ++i) {
    const draco::AttributeValueIndex val_index = pa.mapped_index(i);
    if (!pa.ConvertValue<float>(val_index, values)) {
      return false;
    }
    for (int j = 0; j < components; ++j) {
      out_values[entry_id++] = values[j];
    }
  }
  return true;
}
template <class DracoArrayT, class ValueTypeT>
bool GetAttributeDataForAllPoints(const draco::PointCloud &pc,
                                         const draco::PointAttribute &pa,
                                         draco::DataType draco_signed_type,
                                         draco::DataType draco_unsigned_type,
                                         DracoArrayT &out_values) {
  const int components = pa.num_components();
  const int num_points = pc.num_points();
  const int num_entries = num_points * components;

  /* if ((pa.data_type() == draco_signed_type ||
       pa.data_type() == draco_unsigned_type) &&
      pa.is_mapping_identity()) {
    // Copy values directly to the output vector.
    const ValueTypeT *ptr = reinterpret_cast<const ValueTypeT *>(
        pa.GetAddress(draco::AttributeValueIndex(0)));
    out_values->MoveData({ptr, ptr + num_entries});
    return true;
  } */

  // Copy values one by one.
  std::vector<ValueTypeT> values(components);
  int entry_id = 0;

  out_values.resize(num_entries);
  for (draco::PointIndex i(0); i < num_points; ++i) {
    const draco::AttributeValueIndex val_index = pa.mapped_index(i);
    if (!pa.ConvertValue<ValueTypeT>(val_index, &values[0])) {
      return false;
    }
    for (int j = 0; j < components; ++j) {
      out_values[entry_id++] = values[j];
    }
  }
  return true;
}
bool GetAttributeUInt8ForAllPoints(const draco::PointCloud &pc,
                                            const draco::PointAttribute &pa,
                                            std::vector<unsigned char> &out_values) {
  return GetAttributeDataForAllPoints<std::vector<unsigned char>, uint8_t>(
      pc, pa, draco::DT_INT8, draco::DT_UINT8, out_values);
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
		const std::unique_ptr<draco::Mesh> &mesh = sor.value();
		if (!mesh) {
			auto &status = sor.status();
			std::cout << "failed to load bake: " << status.code() << " " << status.error_msg_string() << std::endl;
		}

		const draco::GeometryMetadata *metadata = mesh->GetMetadata();
		std::string name;
		bool ok = metadata->GetEntryString("name", &name);
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
		  numPositions = numPoints * numComponents;

      GetAttributeFloatForAllPoints(*mesh, *attribute, geometry->positions);
		}
		{
		  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::TEX_COORD);
		  if (attribute) {
			  // int numComponents = attribute->num_components();
			  // int numPoints = mesh->num_points();
			  // int numValues = numPoints * numComponents;

			  GetAttributeFloatForAllPoints(*mesh, *attribute, geometry->uvs);
			} else {
				geometry->uvs.resize(numPositions/3*2);
			}
		}
		{
		  const draco::PointAttribute *attribute = mesh->GetNamedAttribute(draco::GeometryAttribute::COLOR);
		  if (attribute) {
			  // int numComponents = attribute->num_components();
			  // int numPoints = mesh->num_points();
			  // int numValues = numPoints * numComponents;

	      GetAttributeUInt8ForAllPoints(*mesh, *attribute, geometry->colors);
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
	    	geometry->indices[i.value()*3] = face[0].value();
	    	geometry->indices[i.value()*3+1] = face[1].value();
	    	geometry->indices[i.value()*3+2] = face[2].value();
	    }
	  }

	  geometrySet->geometries.push_back(geometry);
	  if (animal) {
	    geometry->aabb.setFromPositions(geometry->positions.data(), geometry->positions.size());
	    Vec center = geometry->aabb.center();
	    Vec size = geometry->aabb.size();
	    geometry->headPivot = center + (size * Vec(0.0f, 1.0f/2.0f * 0.5f, -1.0f/2.0f * 0.5f));
	    Vec legsPivot = center + (size * Vec(0.0f, -1.0f/2.0f + 1.0f/3.0f, 0.0f));

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
		    	head = Vec(0, 0, 0);
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
		    	position = Vec(0, 0, 0);
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

void doGetAnimalGeometry(GeometrySet *geometrySet, unsigned int hash, float **positions, unsigned char **colors, unsigned int **indices, float **heads, float **legs, unsigned int &numPositions, unsigned int &numColors, unsigned int &numIndices, unsigned int &numHeads, unsigned int &numLegs, float *headPivot, float *aabb) {
  unsigned int animalGeometryIndex = (unsigned int)std::floor((float)hash/(float)0xFFFFFF*(float)geometrySet->animalGeometries.size());
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

/* class MarchObject {
public:
	unsigned int id;
	char name[MAX_NAME_LENGTH];
	unsigned int nameLength;
	Vec position;
	Quat quaternion;
};
class SubparcelObject {
public:
	int index;
  char heightfield[SUBPARCEL_SIZE_P1*SUBPARCEL_SIZE_P1*SUBPARCEL_SIZE_P1 + 1]; // align
  unsigned char lightfield[SUBPARCEL_SIZE_P1*SUBPARCEL_SIZE_P1*SUBPARCEL_SIZE_P1 + 1]; // align
}; */
void doGetMarchObjectStats(GeometrySet *geometrySet, Subparcel *subparcel, unsigned int &numPositions, unsigned int &numUvs, unsigned int &numIds, unsigned int &numIndices, unsigned int &numSkyLights, unsigned int &numTorchLights) {
  unsigned int &positionsIndex = numPositions;
  unsigned int &uvsIndex = numUvs;
  unsigned int &idsIndex = numIds;
  unsigned int &indicesIndex = numIndices;
  unsigned int &skyLightsIndex = numSkyLights;
  unsigned int &torchLightsIndex = numTorchLights;

  positionsIndex = 0;
  uvsIndex = 0;
  idsIndex = 0;
  indicesIndex = 0;
  skyLightsIndex = 0;
  torchLightsIndex = 0;

  for (unsigned int i = 0; i < subparcel->numObjects; i++) {
    Object &object = subparcel->objects[i];
    std::string name(object.name);
    auto iter = geometrySet->geometryMap.find(name);
    if (iter != geometrySet->geometryMap.end()) {
    	Geometry *geometry = iter->second;
	    /* if (!geometry) {
	    	std::cout << "failed to get geometry: " << name << std::endl;
	    	abort();
	    } */
	    positionsIndex += geometry->positions.size();
	    uvsIndex += geometry->uvs.size();
	    idsIndex += geometry->positions.size()/3;
	    indicesIndex += geometry->indices.size();
	    skyLightsIndex += geometry->positions.size()/3;
	    torchLightsIndex += geometry->positions.size()/3;
	  }
  }
}
void doMarchObjects(GeometrySet *geometrySet, int x, int y, int z, Subparcel *subparcel, Subparcel *subparcels, unsigned int numSubparcels, float *positions, float *uvs, float *ids, unsigned int *indices, unsigned char *skyLights, unsigned char *torchLights, unsigned int indexOffset) {
  unsigned int positionsIndex = 0;
  unsigned int uvsIndex = 0;
  unsigned int idsIndex = 0;
  unsigned int indicesIndex = 0;
  unsigned int skyLightsIndex = 0;
  unsigned int torchLightsIndex = 0;

  for (unsigned int i = 0; i < subparcel->numObjects; i++) {
    Object &object = subparcel->objects[i];
    std::string name(object.name);
    auto iter = geometrySet->geometryMap.find(name);
    if (iter != geometrySet->geometryMap.end()) {
    	Geometry *geometry = iter->second;
	    /* if (!geometry) {
	    	std::cout << "failed to get geometry: " << name << std::endl;
	    	abort();
	    } */
	    Matrix matrix;
	    matrix.compose(object.position, object.quaternion, Vec{1, 1, 1});

	    unsigned int indexOffset2 = indexOffset + positionsIndex/3;
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
	      int sx = (int)std::floor((float)ax/(float)SUBPARCEL_SIZE);
	      int sy = (int)std::floor((float)ay/(float)SUBPARCEL_SIZE);
	      int sz = (int)std::floor((float)az/(float)SUBPARCEL_SIZE);
	      int subparcelIndex = getSubparcelIndex(sx, sy, sz);
	      Subparcel *subparcel = std::find_if(subparcels, subparcels + numSubparcels, [&](const Subparcel &subparcel) -> bool {
	      	return subparcel.coord.index == subparcelIndex;
	      });
	      if (subparcel != subparcels + numSubparcels) {
	        int lx = ax - SUBPARCEL_SIZE*sx;
	        int ly = ay - SUBPARCEL_SIZE*sy;
	        int lz = az - SUBPARCEL_SIZE*sz;
	        unsigned int fieldIndex = getFieldIndex(lx, ly, lz);
	        skyLights[skyLightsIndex + jOffset] = subparcel->heightfield[fieldIndex] < 0 ? 0 : subparcel->heightfield[fieldIndex];
	        torchLights[torchLightsIndex + jOffset] = subparcel->lightfield[fieldIndex];
	      } else {
	        skyLights[skyLightsIndex + jOffset] = 0;
	        torchLights[torchLightsIndex + jOffset] = 0;
	      }
	    }
	    positionsIndex += geometry->positions.size();
	    skyLightsIndex += geometry->positions.size()/3;
	    torchLightsIndex += geometry->positions.size()/3;

	    memcpy(uvs + uvsIndex, geometry->uvs.data(), geometry->uvs.size()*sizeof(float));
	    uvsIndex += geometry->uvs.size();

	    std::fill(ids + idsIndex, ids + idsIndex + geometry->positions.size()/3, (float)object.id);
	    idsIndex += geometry->positions.size()/3;
	  }
  }
}

std::vector<std::shared_ptr<Subparcel>> doAddObject(Tracker *tracker, OBJECT_TYPE type, const std::string &name, float *position, float *quaternion) {
  const int sdx = (int)std::floor(position[0]/(float)SUBPARCEL_SIZE);
  const int sdy = (int)std::floor(position[1]/(float)SUBPARCEL_SIZE);
  const int sdz = (int)std::floor(position[2]/(float)SUBPARCEL_SIZE);
  int index = getSubparcelIndex(sdx, sdy, sdz);
  
  std::shared_ptr<Subparcel> subparcel;
  {
    std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
    auto subparcelIter = tracker->subparcels.find(index);
    if (subparcelIter != tracker->subparcels.end()) {
      subparcel = subparcelIter->second;
    }
  }
  
  std::vector<std::shared_ptr<Subparcel>> result;
  if (subparcel) {
    Object o;
    o.id = (unsigned int)rand();
    o.type = type;
    strcpy(o.name, name.c_str());
    o.position = Vec{
      position[0],
      position[1],
      position[2],
    };
    o.quaternion = Quat{
      quaternion[0],
      quaternion[1],
      quaternion[2],
      quaternion[3],
    };
    subparcel->objects[subparcel->numObjects++] = o;

    // XXX re-march objects

    result.push_back(subparcel);
  }
  return std::move(result);
}
std::vector<std::shared_ptr<Subparcel>> doRemoveObject(Tracker *tracker, int meshIndex, unsigned int objectId) {
  std::shared_ptr<Subparcel> subparcel;
  {
    std::lock_guard<std::mutex> lock(tracker->subparcelsMutex);
    auto subparcelIter = tracker->subparcels.find(meshIndex);
    if (subparcelIter != tracker->subparcels.end()) {
      subparcel = subparcelIter->second;
    }
  }
  
  std::vector<std::shared_ptr<Subparcel>> result;
  if (subparcel) {
    Object *objectsEnd = ((Object *)subparcel->objects) + subparcel->numObjects;
    auto objectIter = std::find_if(subparcel->objects, objectsEnd, [&](const Object &object) -> bool {
      return object.id == objectId;
    });
    if (objectIter) {
      while ((objectIter + 1) != objectsEnd) {
        *objectIter = *(objectIter + 1);
        objectIter++;
      }
      subparcel->numObjects--;
      
      // re-march objects

      result.push_back(subparcel);
    }
  }
  return std::move(result);
}