mkdir -p bin
emcc --std=c++11 -s WASM=1 -s USE_PTHREADS=1 -O3 -s TOTAL_MEMORY=52428800 \
  objectize.cc vector.cc \
  draco/core/draco_types.cc \
	draco/core/options.cc \
	draco/core/data_buffer.cc \
	draco/compression/decode.cc \
	draco/core/decoder_buffer.cc \
	draco/core/bit_utils.cc \
	draco/core/quantization_utils.cc \
	draco/mesh/mesh.cc \
	draco/compression/mesh/mesh_decoder.cc \
	draco/compression/mesh/mesh_edgebreaker_decoder.cc \
	draco/compression/mesh/mesh_edgebreaker_decoder_impl.cc \
	draco/compression/mesh/mesh_sequential_decoder.cc \
	draco/point_cloud/point_cloud.cc \
	draco/compression/point_cloud/point_cloud_decoder.cc \
	draco/metadata/metadata.cc \
	draco/attributes/geometry_attribute.cc \
	draco/attributes/attribute_octahedron_transform.cc \
	draco/attributes/attribute_quantization_transform.cc \
	draco/attributes/attribute_transform.cc \
	draco/metadata/geometry_metadata.cc \
	draco/metadata/metadata_decoder.cc \
	draco/compression/entropy/symbol_decoding.cc \
	draco/mesh/corner_table.cc \
	draco/mesh/mesh_attribute_corner_table.cc \
	draco/attributes/point_attribute.cc \
	draco/compression/bit_coders/rans_bit_decoder.cc \
	draco/compression/attributes/sequential_normal_attribute_decoder.cc \
	draco/compression/attributes/attributes_decoder.cc \
	draco//compression/attributes/sequential_attribute_decoder.cc \
	draco/compression/attributes/sequential_attribute_decoders_controller.cc \
	draco/compression/attributes/sequential_integer_attribute_decoder.cc \
	draco/compression/attributes/sequential_quantization_attribute_decoder.cc \
  -I. \
  --pre-js prefix.js --post-js postfix.js \
  -o bin/geometry.js
cat prefix2.js bin/geometry.js >bin/geometry2.js
