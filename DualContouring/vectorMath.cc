#include "vectorMath.h"

vm::vec2 vm::min(const vm::vec2 &v1, const vm::vec2 &v2)
{
    return vm::vec2(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
};
vm::vec3 vm::min(const vm::vec3 &v1, const vm::vec3 &v2)
{
    return vm::vec3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
};
vm::vec4 vm::min(const vm::vec4 &v1, const vm::vec4 &v2)
{
    return vm::vec4(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z), std::min(v1.w, v2.w));
};
vm::ivec2 vm::min(const vm::ivec2 &v1, const vm::ivec2 &v2)
{
    return vm::ivec2(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
};
vm::ivec3 vm::min(const vm::ivec3 &v1, const vm::ivec3 &v2)
{
    return vm::ivec3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
};
vm::ivec4 vm::min(const vm::ivec4 &v1, const vm::ivec4 &v2)
{
    return vm::ivec4(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z), std::min(v1.w, v2.w));
};

vm::vec2 vm::min(const vm::vec2 &v1, const float _m)
{
    return vm::vec2(std::min(v1.x, _m), std::min(v1.y, _m));
};
vm::vec3 vm::min(const vm::vec3 &v1, const float _m)
{
    return vm::vec3(std::min(v1.x, _m), std::min(v1.y, _m), std::min(v1.z, _m));
};
vm::vec4 vm::min(const vm::vec4 &v1, const float _m)
{
    return vm::vec4(std::min(v1.x, _m), std::min(v1.y, _m), std::min(v1.z, _m), std::min(v1.w, _m));
};
vm::ivec2 vm::min(const vm::ivec2 &v1, const int _m)
{
    return vm::ivec2(std::min(v1.x, _m), std::min(v1.y, _m));
};
vm::ivec3 vm::min(const vm::ivec3 &v1, const int _m)
{
    return vm::ivec3(std::min(v1.x, _m), std::min(v1.y, _m), std::min(v1.z, _m));
};
vm::ivec4 vm::min(const vm::ivec4 &v1, const int _m)
{
    return vm::ivec4(std::min(v1.x, _m), std::min(v1.y, _m), std::min(v1.z, _m), std::min(v1.w, _m));
};

vm::vec2 vm::max(const vm::vec2 &v1, const vm::vec2 &v2)
{
    return vm::vec2(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
};
vm::vec3 vm::max(const vm::vec3 &v1, const vm::vec3 &v2)
{
    return vm::vec3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
};
vm::vec4 vm::max(const vm::vec4 &v1, const vm::vec4 &v2)
{
    return vm::vec4(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z), std::max(v1.w, v2.w));
};
vm::ivec2 vm::max(const vm::ivec2 &v1, const vm::ivec2 &v2)
{
    return vm::ivec2(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
};
vm::ivec3 vm::max(const vm::ivec3 &v1, const vm::ivec3 &v2)
{
    return vm::ivec3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
};
vm::ivec4 vm::max(const vm::ivec4 &v1, const vm::ivec4 &v2)
{
    return vm::ivec4(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z), std::max(v1.w, v2.w));
};

vm::vec2 vm::max(const vm::vec2 &v1, const float _m)
{
    return vm::vec2(std::max(v1.x, _m), std::max(v1.y, _m));
};
vm::vec3 vm::max(const vm::vec3 &v1, const float _m)
{
    return vm::vec3(std::max(v1.x, _m), std::max(v1.y, _m), std::max(v1.z, _m));
};
vm::vec4 vm::max(const vm::vec4 &v1, const float _m)
{
    return vm::vec4(std::max(v1.x, _m), std::max(v1.y, _m), std::max(v1.z, _m), std::max(v1.w, _m));
};
vm::ivec2 vm::max(const vm::ivec2 &v1, const int _m)
{
    return vm::ivec2(std::max(v1.x, _m), std::max(v1.y, _m));
};
vm::ivec3 vm::max(const vm::ivec3 &v1, const int _m)
{
    return vm::ivec3(std::max(v1.x, _m), std::max(v1.y, _m), std::max(v1.z, _m));
};
vm::ivec4 vm::max(const vm::ivec4 &v1, const int _m)
{
    return vm::ivec4(std::max(v1.x, _m), std::max(v1.y, _m), std::max(v1.z, _m), std::max(v1.w, _m));
};

vm::vec2 vm::abs(const vm::vec2 &v)
{
    return vm::vec2(std::abs(v.x), std::abs(v.y));
}
vm::vec3 vm::abs(const vm::vec3 &v)
{
    return vm::vec3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}
vm::vec4 vm::abs(const vm::vec4 &v)
{
    return vm::vec4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
}
vm::ivec2 vm::abs(const vm::ivec2 &v)
{
    return vm::ivec2(std::abs(v.x), std::abs(v.y));
}
vm::ivec3 vm::abs(const vm::ivec3 &v)
{
    return vm::ivec3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}
vm::ivec4 vm::abs(const vm::ivec4 &v)
{
    return vm::ivec4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
}

vm::vec2 vm::normalize(const vm::vec2 &v)
{
    float length_of_v = std::sqrt((v.x * v.x) + (v.y * v.y));
    return vm::vec2(v.x / length_of_v, v.y / length_of_v);
}
vm::vec3 vm::normalize(const vm::vec3 &v)
{
    float length_of_v = std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
    return vm::vec3(v.x / length_of_v, v.y / length_of_v, v.z / length_of_v);
}
vm::vec4 vm::normalize(const vm::vec4 &v)
{
    float length_of_v = std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
    return vm::vec4(v.x / length_of_v, v.y / length_of_v, v.z / length_of_v, v.w / length_of_v);
}
vm::ivec2 vm::normalize(const vm::ivec2 &v)
{
    float length_of_v = std::sqrt((v.x * v.x) + (v.y * v.y));
    return vm::ivec2(v.x / length_of_v, v.y / length_of_v);
}
vm::ivec3 vm::normalize(const vm::ivec3 &v)
{
    float length_of_v = std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
    return vm::ivec3(v.x / length_of_v, v.y / length_of_v, v.z / length_of_v);
}
vm::ivec4 vm::normalize(const vm::ivec4 &v)
{
    float length_of_v = std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
    return vm::ivec4(v.x / length_of_v, v.y / length_of_v, v.z / length_of_v, v.w / length_of_v);
}

float vm::length(const vm::vec2 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
};
float vm::length(const vm::vec3 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
};
float vm::length(const vm::vec4 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
};
float vm::length(const vm::ivec2 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
};
float vm::length(const vm::ivec3 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
};
float vm::length(const vm::ivec4 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
};
float vm::length(const float v)
{
    return std::sqrt(v * v);
};

float vm::clamp(const float v, const float min, const float max)
{
    return std::max(min, std::min(v, max));
};
int vm::clamp(const int v, const int min, const int max)
{
    return std::max(min, std::min(v, max));
};

// + operator
vm::vec2 vm::operator+(const vm::vec2 &v1, const vm::vec2 &v2)
{
    return vm::vec2(v1.x + v2.x, v1.y + v2.y);
}
vm::vec3 vm::operator+(const vm::vec3 &v1, const vm::vec3 &v2)
{
    return vm::vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
vm::vec4 vm::operator+(const vm::vec4 &v1, const vm::vec4 &v2)
{
    return vm::vec4(v1.x + v2.x,
                    v1.y + v2.y,
                    v1.z + v2.z,
                    v1.w + v2.w);
}
vm::ivec2 vm::operator+(const vm::ivec2 &v1, const vm::ivec2 &v2)
{
    return vm::ivec2(v1.x + v2.x, v1.y + v2.y);
}
vm::ivec3 vm::operator+(const vm::ivec3 &v1, const vm::ivec3 &v2)
{
    return vm::ivec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
vm::ivec4 vm::operator+(const vm::ivec4 &v1, const vm::ivec4 &v2)
{
    return vm::ivec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

vm::vec2 vm::operator+(const vm::vec2 &v1, const float _m)
{
    return vm::vec2(v1.x + _m, v1.y + _m);
}
vm::vec3 vm::operator+(const vm::vec3 &v1, const float _m)
{
    return vm::vec3(v1.x + _m, v1.y + _m, v1.z + _m);
}
vm::vec4 vm::operator+(const vm::vec4 &v1, const float _m)
{
    return vm::vec4(v1.x + _m,
                    v1.y + _m,
                    v1.z + _m,
                    v1.w + _m);
}
vm::ivec2 vm::operator+(const vm::ivec2 &v1, const int _m)
{
    return vm::ivec2(v1.x + _m, v1.y + _m);
}
vm::ivec3 vm::operator+(const vm::ivec3 &v1, const int _m)
{
    return vm::ivec3(v1.x + _m, v1.y + _m, v1.z + _m);
}
vm::ivec4 vm::operator+(const vm::ivec4 &v1, const int _m)
{
    return vm::ivec4(v1.x + _m, v1.y + _m, v1.z + _m, v1.w + _m);
}
// - operator
vm::vec2 vm::operator-(const vm::vec2 &v1, const vm::vec2 &v2)
{
    return vm::vec2(v1.x - v2.x, v1.y - v2.y);
}
vm::vec3 vm::operator-(const vm::vec3 &v1, const vm::vec3 &v2)
{
    return vm::vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
vm::vec4 vm::operator-(const vm::vec4 &v1, const vm::vec4 &v2)
{
    return vm::vec4(v1.x - v2.x,
                    v1.y - v2.y,
                    v1.z - v2.z,
                    v1.w - v2.w);
}
vm::ivec2 vm::operator-(const vm::ivec2 &v1, const vm::ivec2 &v2)
{

    return vm::ivec2(v1.x - v2.x,
                     v1.y - v2.y);
}
vm::ivec3 vm::operator-(const vm::ivec3 &v1, const vm::ivec3 &v2)
{
    return vm::ivec3(v1.x - v2.x,
                     v1.y - v2.y,
                     v1.z - v2.z);
}
vm::ivec4 vm::operator-(const vm::ivec4 &v1, const vm::ivec4 &v2)
{
    return vm::ivec4(v1.x - v2.x,
                     v1.y - v2.y,
                     v1.z - v2.z,
                     v1.w - v2.w);
}

vm::vec2 vm::operator-(const vm::vec2 &v1, const float _m)
{
    return vm::vec2(v1.x - _m, v1.y - _m);
}
vm::vec3 vm::operator-(const vm::vec3 &v1, const float _m)
{
    return vm::vec3(v1.x - _m, v1.y - _m, v1.z - _m);
}
vm::vec4 vm::operator-(const vm::vec4 &v1, const float _m)
{
    return vm::vec4(v1.x - _m,
                    v1.y - _m,
                    v1.z - _m,
                    v1.w - _m);
}
vm::ivec2 vm::operator-(const vm::ivec2 &v1, const int _m)
{

    return vm::ivec2(v1.x - _m,
                     v1.y - _m);
}
vm::ivec3 vm::operator-(const vm::ivec3 &v1, const int _m)
{
    return vm::ivec3(v1.x - _m,
                     v1.y - _m,
                     v1.z - _m);
}
vm::ivec4 vm::operator-(const vm::ivec4 &v1, const int _m)
{
    return vm::ivec4(v1.x - _m,
                     v1.y - _m,
                     v1.z - _m,
                     v1.w - _m);
}
// * operator
vm::vec2 vm::operator*(const vm::vec2 &v1, const vm::vec2 &v2)
{
    return vm::vec2(v1.x * v2.x, v1.y * v2.y);
}
vm::vec3 vm::operator*(const vm::vec3 &v1, const vm::vec3 &v2)
{
    return vm::vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
vm::vec4 vm::operator*(const vm::vec4 &v1, const vm::vec4 &v2)
{
    return vm::vec4(v1.x * v2.x,
                    v1.y * v2.y,
                    v1.z * v2.z,
                    v1.w * v2.w);
}
vm::ivec2 vm::operator*(const vm::ivec2 &v1, const vm::ivec2 &v2)
{

    return vm::ivec2(v1.x * v2.x,
                     v1.y * v2.y);
}
vm::ivec3 vm::operator*(const vm::ivec3 &v1, const vm::ivec3 &v2)
{
    return vm::ivec3(v1.x * v2.x,
                     v1.y * v2.y,
                     v1.z * v2.z);
}
vm::ivec4 vm::operator*(const vm::ivec4 &v1, const vm::ivec4 &v2)
{
    return vm::ivec4(v1.x * v2.x,
                     v1.y * v2.y,
                     v1.z * v2.z,
                     v1.w * v2.w);
}

vm::vec2 vm::operator*(const vm::vec2 &v1, const float _m)
{
    return vm::vec2(v1.x * _m, v1.y * _m);
}
vm::vec3 vm::operator*(const vm::vec3 &v1, const float _m)
{
    return vm::vec3(v1.x * _m, v1.y * _m, v1.z * _m);
}
vm::vec4 vm::operator*(const vm::vec4 &v1, const float _m)
{
    return vm::vec4(v1.x * _m,
                    v1.y * _m,
                    v1.z * _m,
                    v1.w * _m);
}
vm::ivec2 vm::operator*(const vm::ivec2 &v1, const int _m)
{

    return vm::ivec2(v1.x * _m,
                     v1.y * _m);
}
vm::ivec3 vm::operator*(const vm::ivec3 &v1, const int _m)
{
    return vm::ivec3(v1.x * _m,
                     v1.y * _m,
                     v1.z * _m);
}
vm::ivec4 vm::operator*(const vm::ivec4 &v1, const int _m)
{
    return vm::ivec4(v1.x * _m,
                     v1.y * _m,
                     v1.z * _m,
                     v1.w * _m);
}
// / operator
vm::vec2 vm::operator/(const vm::vec2 &v1, const vm::vec2 &v2)
{
    return vm::vec2(v1.x / v2.x, v1.y / v2.y);
}
vm::vec3 vm::operator/(const vm::vec3 &v1, const vm::vec3 &v2)
{
    return vm::vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}
vm::vec4 vm::operator/(const vm::vec4 &v1, const vm::vec4 &v2)
{
    return vm::vec4(v1.x / v2.x,
                    v1.y / v2.y,
                    v1.z / v2.z,
                    v1.w / v2.w);
}
vm::ivec2 vm::operator/(const vm::ivec2 &v1, const vm::ivec2 &v2)
{

    return vm::ivec2(v1.x / v2.x,
                     v1.y / v2.y);
}
vm::ivec3 vm::operator/(const vm::ivec3 &v1, const vm::ivec3 &v2)
{
    return vm::ivec3(v1.x / v2.x,
                     v1.y / v2.y,
                     v1.z / v2.z);
}
vm::ivec4 vm::operator/(const vm::ivec4 &v1, const vm::ivec4 &v2)
{
    return vm::ivec4(v1.x / v2.x,
                     v1.y / v2.y,
                     v1.z / v2.z,
                     v1.w / v2.w);
}

vm::vec2 vm::operator/(const vm::vec2 &v1, const float _m)
{
    return vm::vec2(v1.x / _m, v1.y / _m);
}
vm::vec3 vm::operator/(const vm::vec3 &v1, const float _m)
{
    return vm::vec3(v1.x / _m, v1.y / _m, v1.z / _m);
}
vm::vec4 vm::operator/(const vm::vec4 &v1, const float _m)
{
    return vm::vec4(v1.x / _m,
                    v1.y / _m,
                    v1.z / _m,
                    v1.w / _m);
}
vm::ivec2 vm::operator/(const vm::ivec2 &v1, const int _m)
{
    return vm::ivec2(v1.x / _m,
                     v1.y / _m);
}
vm::ivec3 vm::operator/(const vm::ivec3 &v1, const int _m)
{
    return vm::ivec3(v1.x / _m,
                     v1.y / _m,
                     v1.z / _m);
}
vm::ivec4 vm::operator/(const vm::ivec4 &v1, const int _m)
{
    return vm::ivec4(v1.x / _m,
                     v1.y / _m,
                     v1.z / _m,
                     v1.w / _m);
}
// += operator
vm::vec2 vm::operator+=(vm::vec2 &v1, const vm::vec2 &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}
vm::vec3 vm::operator+=(vm::vec3 &v1, const vm::vec3 &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}
vm::vec4 vm::operator+=(vm::vec4 &v1, const vm::vec4 &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    v1.w += v2.w;
    return v1;
}
vm::ivec2 vm::operator+=(vm::ivec2 &v1, const vm::ivec2 &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}
vm::ivec3 vm::operator+=(vm::ivec3 &v1, const vm::ivec3 &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

vm::ivec4 vm::operator+=(vm::ivec4 &v1, const float _m)
{
    v1.x += _m;
    v1.y += _m;
    v1.z += _m;
    v1.w += _m;
    return v1;
}
vm::vec2 vm::operator+=(vm::vec2 &v1, const float _m)
{
    v1.x += _m;
    v1.y += _m;
    return v1;
}
vm::vec3 vm::operator+=(vm::vec3 &v1, const float _m)
{
    v1.x += _m;
    v1.y += _m;
    v1.z += _m;
    return v1;
}
vm::vec4 vm::operator+=(vm::vec4 &v1, const float _m)
{
    v1.x += _m;
    v1.y += _m;
    v1.z += _m;
    v1.w += _m;
    return v1;
}
vm::ivec2 vm::operator+=(vm::ivec2 &v1, const int _m)
{
    v1.x += _m;
    v1.y += _m;
    return v1;
}
vm::ivec3 vm::operator+=(vm::ivec3 &v1, const int _m)
{
    v1.x += _m;
    v1.y += _m;
    v1.z += _m;
    return v1;
}
vm::ivec4 vm::operator+=(vm::ivec4 &v1, const int _m)
{
    v1.x += _m;
    v1.y += _m;
    v1.z += _m;
    v1.w += _m;
    return v1;
}
// -= operator
vm::vec2 vm::operator-=(vm::vec2 &v1, const vm::vec2 &v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    return v1;
}
vm::vec3 vm::operator-=(vm::vec3 &v1, const vm::vec3 &v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}
vm::vec4 vm::operator-=(vm::vec4 &v1, const vm::vec4 &v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    v1.w -= v2.w;
    return v1;
}
vm::ivec2 vm::operator-=(vm::ivec2 &v1, const vm::ivec2 &v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    return v1;
}
vm::ivec3 vm::operator-=(vm::ivec3 &v1, const vm::ivec3 &v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}

vm::ivec4 vm::operator-=(vm::ivec4 &v1, const float _m)
{
    v1.x -= _m;
    v1.y -= _m;
    v1.z -= _m;
    v1.w -= _m;
    return v1;
}
vm::vec2 vm::operator-=(vm::vec2 &v1, const float _m)
{
    v1.x -= _m;
    v1.y -= _m;
    return v1;
}
vm::vec3 vm::operator-=(vm::vec3 &v1, const float _m)
{
    v1.x -= _m;
    v1.y -= _m;
    v1.z -= _m;
    return v1;
}
vm::vec4 vm::operator-=(vm::vec4 &v1, const float _m)
{
    v1.x -= _m;
    v1.y -= _m;
    v1.z -= _m;
    v1.w -= _m;
    return v1;
}
vm::ivec2 vm::operator-=(vm::ivec2 &v1, const int _m)
{
    v1.x -= _m;
    v1.y -= _m;
    return v1;
}
vm::ivec3 vm::operator-=(vm::ivec3 &v1, const int _m)
{
    v1.x -= _m;
    v1.y -= _m;
    v1.z -= _m;
    return v1;
}
vm::ivec4 vm::operator-=(vm::ivec4 &v1, const int _m)
{
    v1.x -= _m;
    v1.y -= _m;
    v1.z -= _m;
    v1.w -= _m;
    return v1;
}
// *= operator
vm::vec2 vm::operator*=(vm::vec2 &v1, const vm::vec2 &v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    return v1;
}
vm::vec3 vm::operator*=(vm::vec3 &v1, const vm::vec3 &v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    return v1;
}
vm::vec4 vm::operator*=(vm::vec4 &v1, const vm::vec4 &v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    v1.w *= v2.w;
    return v1;
}
vm::ivec2 vm::operator*=(vm::ivec2 &v1, const vm::ivec2 &v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    return v1;
}
vm::ivec3 vm::operator*=(vm::ivec3 &v1, const vm::ivec3 &v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    return v1;
}

vm::ivec4 vm::operator*=(vm::ivec4 &v1, const float _m)
{
    v1.x *= _m;
    v1.y *= _m;
    v1.z *= _m;
    v1.w *= _m;
    return v1;
}
vm::vec2 vm::operator*=(vm::vec2 &v1, const float _m)
{
    v1.x *= _m;
    v1.y *= _m;
    return v1;
}
vm::vec3 vm::operator*=(vm::vec3 &v1, const float _m)
{
    v1.x *= _m;
    v1.y *= _m;
    v1.z *= _m;
    return v1;
}
vm::vec4 vm::operator*=(vm::vec4 &v1, const float _m)
{
    v1.x *= _m;
    v1.y *= _m;
    v1.z *= _m;
    v1.w *= _m;
    return v1;
}
vm::ivec2 vm::operator*=(vm::ivec2 &v1, const int _m)
{
    v1.x *= _m;
    v1.y *= _m;
    return v1;
}
vm::ivec3 vm::operator*=(vm::ivec3 &v1, const int _m)
{
    v1.x *= _m;
    v1.y *= _m;
    v1.z *= _m;
    return v1;
}
vm::ivec4 vm::operator*=(vm::ivec4 &v1, const int _m)
{
    v1.x *= _m;
    v1.y *= _m;
    v1.z *= _m;
    v1.w *= _m;
    return v1;
}
// /= operator
vm::vec2 vm::operator/=(vm::vec2 &v1, const vm::vec2 &v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    return v1;
}
vm::vec3 vm::operator/=(vm::vec3 &v1, const vm::vec3 &v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;
    return v1;
}
vm::vec4 vm::operator/=(vm::vec4 &v1, const vm::vec4 &v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;
    v1.w /= v2.w;
    return v1;
}
vm::ivec2 vm::operator/=(vm::ivec2 &v1, const vm::ivec2 &v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    return v1;
}
vm::ivec3 vm::operator/=(vm::ivec3 &v1, const vm::ivec3 &v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;
    return v1;
}

vm::ivec4 vm::operator/=(vm::ivec4 &v1, const float _m)
{
    v1.x /= _m;
    v1.y /= _m;
    v1.z /= _m;
    v1.w /= _m;
    return v1;
}
vm::vec2 vm::operator/=(vm::vec2 &v1, const float _m)
{
    v1.x /= _m;
    v1.y /= _m;
    return v1;
}
vm::vec3 vm::operator/=(vm::vec3 &v1, const float _m)
{
    v1.x /= _m;
    v1.y /= _m;
    v1.z /= _m;
    return v1;
}
vm::vec4 vm::operator/=(vm::vec4 &v1, const float _m)
{
    v1.x /= _m;
    v1.y /= _m;
    v1.z /= _m;
    v1.w /= _m;
    return v1;
}
vm::ivec2 vm::operator/=(vm::ivec2 &v1, const int _m)
{
    v1.x /= _m;
    v1.y /= _m;
    return v1;
}
vm::ivec3 vm::operator/=(vm::ivec3 &v1, const int _m)
{
    v1.x /= _m;
    v1.y /= _m;
    v1.z /= _m;
    return v1;
}
vm::ivec4 vm::operator/=(vm::ivec4 &v1, const int _m)
{
    v1.x /= _m;
    v1.y /= _m;
    v1.z /= _m;
    v1.w /= _m;
    return v1;
}

// == operator
bool vm::operator==(const vm::vec2 &v1, const vm::vec2 &v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y);
}
bool vm::operator==(const vm::vec3 &v1, const vm::vec3 &v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}
bool vm::operator==(const vm::vec4 &v1, const vm::vec4 &v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
}
bool vm::operator==(const vm::ivec2 &v1, const vm::ivec2 &v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y);
}
bool vm::operator==(const vm::ivec3 &v1, const vm::ivec3 &v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
}
bool vm::operator==(const vm::ivec4 &v1, const vm::ivec4 &v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
}

// % operator
vm::ivec2 vm::operator%(const vm::ivec2 &v1, const vm::ivec2 &v2)
{
    return vm::ivec2(v1.x % v2.x,
                     v1.y % v2.y);
}
vm::ivec3 vm::operator%(const vm::ivec3 &v1, const vm::ivec3 &v2)
{
    return vm::ivec3(v1.x % v2.x,
                     v1.y % v2.y,
                     v1.z % v2.z);
}
vm::ivec4 vm::operator%(const vm::ivec4 &v1, const vm::ivec4 &v2)
{
    return vm::ivec4(v1.x % v2.x,
                     v1.y % v2.y,
                     v1.z % v2.z,
                     v1.w % v2.w);
}
vm::ivec2 vm::operator%(const vm::ivec2 &v1, const int _m)
{
    return vm::ivec2(v1.x % _m,
                     v1.y % _m);
}
vm::ivec3 vm::operator%(const vm::ivec3 &v1, const int _m)
{
    return vm::ivec3(v1.x % _m,
                     v1.y % _m, v1.z % _m);
}
vm::ivec4 vm::operator%(const vm::ivec4 &v1, const int _m)
{
    return vm::ivec4(v1.x % _m,
                     v1.y % _m, v1.z % _m, v1.w % _m);
}
