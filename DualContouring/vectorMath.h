#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include <cmath>
#include <algorithm>
#include <cstdio>

namespace vm
{
    struct vec2
    {
        vec2()
        {
            x = 0;
            y = 0;
        }
        vec2(const float &_x, const float &_y)
        {
            x = _x;
            y = _y;
        }
        vec2(const float &_v)
        {
            x = _v;
            y = _v;
        }
        float x;
        float y;
    };
    struct vec3
    {
        vec3()
        {
            x = 0;
            y = 0;
            z = 0;
        }
        vec3(const float &_x, const float &_y, const float &_z)
        {
            x = _x;
            y = _y;
            z = _z;
        }
        vec3(const float &_v)
        {
            x = _v;
            y = _v;
            z = _v;
        }
        float x;
        float y;
        float z;
    };
    struct vec4
    {
        vec4()
        {
            x = 0;
            y = 0;
            z = 0;
            w = 0;
        }
        vec4(const float &_x, const float &_y, const float &_z, const float &_w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        }
        vec4(const float &_v)
        {
            x = _v;
            y = _v;
            z = _v;
            w = _v;
        }
        float x;
        float y;
        float z;
        float w;
    };
    struct ivec2
    {
        ivec2()
        {
            x = 0;
            y = 0;
        }
        ivec2(const int &_x, const int &_y)
        {
            x = _x;
            y = _y;
        }
        ivec2(const int &_v)
        {
            x = _v;
            y = _v;
        }
        int x;
        int y;
    };
    struct ivec3
    {
        ivec3()
        {
            x = 0;
            y = 0;
            z = 0;
        }
        ivec3(const int &_x, const int &_y, const int &_z)
        {
            x = _x;
            y = _y;
            z = _z;
        }
        ivec3(const int _v)
        {
            x = _v;
            y = _v;
            z = _v;
        }
        int x;
        int y;
        int z;
    };
    struct ivec4
    {
        ivec4()
        {
            x = 0;
            y = 0;
            z = 0;
            w = 0;
        }
        ivec4(const int &_x, const int &_y, const int &_z, const int &_w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        }
        ivec4(const int &_v)
        {
            x = _v;
            y = _v;
            z = _v;
            w = _v;
        }
        int x;
        int y;
        int z;
        int w;
    };

    vec2 min(const vec2 &v1, const vec2 &v2);
    vec3 min(const vec3 &v1, const vec3 &v2);
    vec4 min(const vec4 &v1, const vec4 &v2);
    ivec2 min(const ivec2 &v1, const ivec2 &v2);
    ivec3 min(const ivec3 &v1, const ivec3 &v2);
    ivec4 min(const ivec4 &v1, const ivec4 &v2);

    vec2 min(const vec2 &v1, const float _m);
    vec3 min(const vec3 &v1, const float _m);
    vec4 min(const vec4 &v1, const float _m);
    ivec2 min(const ivec2 &v1, const int _m);
    ivec3 min(const ivec3 &v1, const int _m);
    ivec4 min(const ivec4 &v1, const int _m);

    vec2 max(const vec2 &v1, const vec2 &v2);
    vec3 max(const vec3 &v1, const vec3 &v2);
    vec4 max(const vec4 &v1, const vec4 &v2);
    ivec2 max(const ivec2 &v1, const ivec2 &v2);
    ivec3 max(const ivec3 &v1, const ivec3 &v2);
    ivec4 max(const ivec4 &v1, const ivec4 &v2);

    vec2 max(const vec2 &v1, const float _m);
    vec3 max(const vec3 &v1, const float _m);
    vec4 max(const vec4 &v1, const float _m);
    ivec2 max(const ivec2 &v1, const int _m);
    ivec3 max(const ivec3 &v1, const int _m);
    ivec4 max(const ivec4 &v1, const int _m);

    vec2 abs(const vec2 &v);
    vec3 abs(const vec3 &v);
    vec4 abs(const vec4 &v);
    ivec2 abs(const ivec2 &v);
    ivec3 abs(const ivec3 &v);
    ivec4 abs(const ivec4 &v);

    vec2 normalize(const vec2 &v);
    vec3 normalize(const vec3 &v);
    vec4 normalize(const vec4 &v);
    ivec2 normalize(const ivec2 &v);
    ivec3 normalize(const ivec3 &v);
    ivec4 normalize(const ivec4 &v);

    float length(const vec2 v);
    float length(const vec3 v);
    float length(const vec4 v);
    float length(const ivec2 v);
    float length(const ivec3 v);
    float length(const ivec4 v);
    float length(const float v);

    float distance(const vm::vec2 &v, const vm::vec2 &o);
    float distance(const vm::vec2 &v, const vm::ivec2 &o);
    float distance(const vm::ivec2 &v, const vm::ivec2 &o);

    float clamp(const float v, const float min, const float max);
    int clamp(const int v, const int min, const int max);

    // + operator
    vec2 operator+(const vec2 &v1, const vec2 &v2);
    vec3 operator+(const vec3 &v1, const vec3 &v2);
    vec4 operator+(const vec4 &v1, const vec4 &v2);
    ivec2 operator+(const ivec2 &v1, const ivec2 &v2);
    ivec3 operator+(const ivec3 &v1, const ivec3 &v2);
    ivec4 operator+(const ivec4 &v1, const ivec4 &v2);

    vec2 operator+(const vec2 &v1, const float _m);
    vec3 operator+(const vec3 &v1, const float _m);
    vec4 operator+(const vec4 &v1, const float _m);
    ivec2 operator+(const ivec2 &v1, const int _m);
    ivec3 operator+(const ivec3 &v1, const int _m);
    ivec4 operator+(const ivec4 &v1, const int _m);
    // - operator
    vec2 operator-(const vec2 &v1, const vec2 &v2);
    vec3 operator-(const vec3 &v1, const vec3 &v2);
    vec4 operator-(const vec4 &v1, const vec4 &v2);
    ivec2 operator-(const ivec2 &v1, const ivec2 &v2);
    ivec3 operator-(const ivec3 &v1, const ivec3 &v2);
    ivec4 operator-(const ivec4 &v1, const ivec4 &v2);

    vec2 operator-(const vec2 &v1, const float _m);
    vec3 operator-(const vec3 &v1, const float _m);

    vec4 operator-(const vec4 &v1, const float _m);
    ivec2 operator-(const ivec2 &v1, const int _m);
    ivec3 operator-(const ivec3 &v1, const int _m);
    ivec4 operator-(const ivec4 &v1, const int _m);

    // * operator
    vec2 operator*(const vec2 &v1, const vec2 &v2);
    vec3 operator*(const vec3 &v1, const vec3 &v2);
    vec4 operator*(const vec4 &v1, const vec4 &v2);
    ivec2 operator*(const ivec2 &v1, const ivec2 &v2);
    ivec3 operator*(const ivec3 &v1, const ivec3 &v2);
    ivec4 operator*(const ivec4 &v1, const ivec4 &v2);
    vec2 operator*(const vec2 &v1, const float _m);
    vec3 operator*(const vec3 &v1, const float _m);
    vec4 operator*(const vec4 &v1, const float _m);
    ivec2 operator*(const ivec2 &v1, const int _m);
    ivec3 operator*(const ivec3 &v1, const int _m);
    ivec4 operator*(const ivec4 &v1, const int _m);

    // / operator
    vec2 operator/(const vec2 &v1, const vec2 &v2);
    vec3 operator/(const vec3 &v1, const vec3 &v2);
    vec4 operator/(const vec4 &v1, const vec4 &v2);
    ivec2 operator/(const ivec2 &v1, const ivec2 &v2);
    ivec3 operator/(const ivec3 &v1, const ivec3 &v2);
    ivec4 operator/(const ivec4 &v1, const ivec4 &v2);
    vec2 operator/(const vec2 &v1, const float _m);
    vec3 operator/(const vec3 &v1, const float _m);
    vec4 operator/(const vec4 &v1, const float _m);
    ivec2 operator/(const ivec2 &v1, const int _m);
    ivec3 operator/(const ivec3 &v1, const int _m);
    ivec4 operator/(const ivec4 &v1, const int _m);

    // += operator
    vec2 operator+=(vec2 &v1, const vec2 &v2);
    vec3 operator+=(vec3 &v1, const vec3 &v2);
    vec4 operator+=(vec4 &v1, const vec4 &v2);
    ivec2 operator+=(ivec2 &v1, const ivec2 &v2);
    ivec3 operator+=(ivec3 &v1, const ivec3 &v2);
    ivec4 operator+=(ivec4 &v1, const float _m);
    vec2 operator+=(vec2 &v1, const float _m);
    vec3 operator+=(vec3 &v1, const float _m);
    vec4 operator+=(vec4 &v1, const float _m);
    ivec2 operator+=(ivec2 &v1, const int _m);
    ivec3 operator+=(ivec3 &v1, const int _m);
    ivec4 operator+=(ivec4 &v1, const int _m);
    // -= operator
    vec2 operator-=(vec2 &v1, const vec2 &v2);
    vec3 operator-=(vec3 &v1, const vec3 &v2);
    vec4 operator-=(vec4 &v1, const vec4 &v2);
    ivec2 operator-=(ivec2 &v1, const ivec2 &v2);
    ivec3 operator-=(ivec3 &v1, const ivec3 &v2);
    ivec4 operator-=(ivec4 &v1, const float _m);
    vec2 operator-=(vec2 &v1, const float _m);
    vec3 operator-=(vec3 &v1, const float _m);
    vec4 operator-=(vec4 &v1, const float _m);
    ivec2 operator-=(ivec2 &v1, const int _m);
    ivec3 operator-=(ivec3 &v1, const int _m);
    ivec4 operator-=(ivec4 &v1, const int _m);
    // *= operator
    vec2 operator*=(vec2 &v1, const vec2 &v2);
    vec3 operator*=(vec3 &v1, const vec3 &v2);
    vec4 operator*=(vec4 &v1, const vec4 &v2);
    ivec2 operator*=(ivec2 &v1, const ivec2 &v2);
    ivec3 operator*=(ivec3 &v1, const ivec3 &v2);
    ivec4 operator*=(ivec4 &v1, const float _m);
    vec2 operator*=(vec2 &v1, const float _m);
    vec3 operator*=(vec3 &v1, const float _m);
    vec4 operator*=(vec4 &v1, const float _m);
    ivec2 operator*=(ivec2 &v1, const int _m);
    ivec3 operator*=(ivec3 &v1, const int _m);
    ivec4 operator*=(ivec4 &v1, const int _m);
    // /= operator
    vec2 operator/=(vec2 &v1, const vec2 &v2);
    vec3 operator/=(vec3 &v1, const vec3 &v2);
    vec4 operator/=(vec4 &v1, const vec4 &v2);
    ivec2 operator/=(ivec2 &v1, const ivec2 &v2);
    ivec3 operator/=(ivec3 &v1, const ivec3 &v2);
    ivec4 operator/=(ivec4 &v1, const float _m);
    vec2 operator/=(vec2 &v1, const float _m);
    vec3 operator/=(vec3 &v1, const float _m);
    vec4 operator/=(vec4 &v1, const float _m);
    ivec2 operator/=(ivec2 &v1, const int _m);
    ivec3 operator/=(ivec3 &v1, const int _m);
    ivec4 operator/=(ivec4 &v1, const int _m);
    // == operator
    bool operator==(const vec2 &v1, const vec2 &v2);
    bool operator==(const vec3 &v1, const vec3 &v2);
    bool operator==(const vec4 &v1, const vec4 &v2);
    bool operator==(const ivec2 &v1, const ivec2 &v2);
    bool operator==(const ivec3 &v1, const ivec3 &v2);
    bool operator==(const ivec4 &v1, const ivec4 &v2);

    // % operator
    ivec2 operator%(const ivec2 &v1, const ivec2 &v2);
    ivec3 operator%(const ivec3 &v1, const ivec3 &v2);
    ivec4 operator%(const ivec4 &v1, const ivec4 &v2);
    ivec2 operator%(const ivec2 &v1, const int _m);
    ivec3 operator%(const ivec3 &v1, const int _m);
    ivec4 operator%(const ivec4 &v1, const int _m);

} // namespace vm

#endif // VECTOR_MATH_H