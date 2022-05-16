#include <emscripten/bind.h>
#include <DualContouring/density.h>
#include <DualContouring/main.h>
#include <vector>

/**
 * Determines whether an JS value is a specified type.
 */
inline bool is_type(emscripten::val value, const std::string &type)
{
    return value.typeOf().as<std::string>() == type;
}

inline bool is_vector(emscripten::val value)
{
    return is_type(value["size"], "function");
}

template <typename T>
std::vector<T> to_vector(emscripten::val v)
{
    std::vector<T> rv;

    if (v.isArray()) {
        rv = emscripten::vecFromJSArray<T>(v);
    } else if (is_vector(v)) {
        rv = v.as<std::vector<T>>();
    } else {
        // Allow single values as well
        rv = {v.as<T>()};
    }

    return rv;
}

float lerp(float s, float e, float t){return s+(e-s)*t;}
float blerp(float c00, float c10, float c01, float c11, float tx, float ty)
{
    return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

class HeightMap
{
public:
    HeightMap(){};
    std::vector<uint8_t> data;
    size_t width{0};
    size_t height{0};
    float metersPerPixel{1};
    float depth{30};
    void setData(emscripten::val jsdata)
    {
        data = to_vector<uint8_t>(jsdata);
    }

    emscripten::val getData() const
    {
        emscripten::val ret(data);
        return ret;
    }

    float getValueAt(float x, float y) const
    {
        if(x < 0 || x > width || y < 0 || y > height) return 0.0;
        auto x1 = floor(x);
        auto x2 = ceil(x);
        auto y1 = floor(y);
        auto y2 = ceil(y);
        return blerp(
            data[y1 * width + x1], 
            data[y1 * width + x2], 
            data[y2 * width + x1], 
            data[y2 * width + x2], x - x1, y - y1);
    }
};

class HeightMapMeshGenerator
{
    HeightMap map;
    std::function<float(float, float)> heightFunc;
public:
    using pointerType = int;
    HeightMapMeshGenerator(const HeightMap& heightMap) : map(heightMap)
    {
        heightFunc = [this](float x, float y)
        {
            int xpix = map.width/2 + x / map.metersPerPixel;
            int ypix = map.height/2 + y / map.metersPerPixel;
            if(xpix > 0 && xpix < map.width && ypix > 0 && ypix < map.height)
                return map.getValueAt(x / map.metersPerPixel, y / map.metersPerPixel) * (map.depth / 256.0 );
            return 0.0;
        };
        
    }

    pointerType generateChunk(float x, float y, float z) const
    {
        DensityData::GetInstance().SetHeightFunction(heightFunc);
        auto buffPtr = (pointerType)DualContouring::createChunk(x, y, z);
        DensityData::GetInstance().SetHeightFunction(nullptr);
        return buffPtr;
    }

    pointerType generateSeam(float x, float y, float z) const
    {
        DensityData::GetInstance().SetHeightFunction(heightFunc);
        auto buffPtr = (pointerType)DualContouring::createSeam(x, y, z);
        DensityData::GetInstance().SetHeightFunction(nullptr);
        return buffPtr;
    }
};

EMSCRIPTEN_BINDINGS(HeighMapGeometry)
{
    emscripten::class_<HeightMap>("HeightMap")
        .constructor()
        .property("data", &HeightMap::getData, &HeightMap::setData)
        .property("width", &HeightMap::width)
        .property("height", &HeightMap::height)
        .property("metersPerPixel", &HeightMap::metersPerPixel)
        .property("depth", &HeightMap::depth);
    
    emscripten::class_<HeightMapMeshGenerator>("HeightMapMeshGenerator")
        .constructor<HeightMap>()
        .function("generateChunk", &HeightMapMeshGenerator::generateChunk)
        .function("generateSeam", &HeightMapMeshGenerator::generateSeam);

    emscripten::register_vector<uint8_t>("vector<uint8_t>");
}