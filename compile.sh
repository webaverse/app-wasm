mkdir -p bin
emcc --std=c++11 -s WASM=1 -s USE_PTHREADS=1 -O3 -s TOTAL_MEMORY=299958272 \
  FastNoise.cpp noise.cc march.cc vector.cc biomes.cc objectize.cc \
  --pre-js prefix.js --post-js postfix.js \
  -o bin/objectize.js
cat prefix2.js bin/objectize.js >bin/objectize2.js
