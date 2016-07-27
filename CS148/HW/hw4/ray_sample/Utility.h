// All utility functions and base structs.

#ifndef _UTILITY_H
#define _UTILITY_H
#define _USE_MATH_DEFINES
#include <cmath>
#include "SimpleImage.h"
#include "Utility.h"
#include <cstdlib>
#include <ctime>
#include <memory>
#include <Windows.h>
#include <iostream>

using namespace std;

class Surface;
struct Vector3f;

const float RAY_T0 = 0.0001f;
const float RAY_T1 = 1000.0f;

// #define DEBUG_DIFFUSE
// Each light source is a 4x4 grids.
#define LIGHT_SAMPLES      16   
// Diffuse Reflection samples at ecliptic.
#define ECLIPTIC_SAMPLES   8
// Diffuse Reflection samples in the upper hemisphere.
#define HEMISPHERE_SAMPLES 4

extern bool fUseFastShading;

struct Point3f {
    union {
        struct {
            float x;
            float y;
            float z;
            float pad;
        };
        __m128 SSE_operand;
    };

    Point3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
    Point3f() : x(0), y(0), z(0), pad(0) { }

    inline bool operator==(const Point3f& p) {
        return x == p.x && y == p.y && z == p.z;
    }

    inline bool operator!=(const Point3f& p) {
        return x != p.x || y != p.y || z != p.z;
    }

    inline Point3f operator-() const {
        return Point3f(-x, -y, -z);
    }

    inline Point3f operator+(const Point3f& add) const {
#ifdef __ENABLE_SSE_POINT__
    __m128 SSEa = SSE_operand;
    __m128 SSEb = add.SSE_operand;

    __declspec(align(16)) Point3f rPoint3f;

    rPoint3f.SSE_operand = _mm_add_ps(SSEa, SSEb);

    return rPoint3f;
#else
    return Point3f(x + add.x, y + add.y, z + add.z);
#endif
    }

    Point3f operator-(const Point3f& sub) const {
#ifdef __ENABLE_SSE_POINT__
    __m128 SSEa = SSE_operand;
    __m128 SSEb = sub.SSE_operand;

    __declspec(align(16)) Point3f rPoint3f;

    rPoint3f.SSE_operand = _mm_sub_ps(SSEa, SSEb);

    return rPoint3f;
#else
    return Point3f(x - sub.x, y - sub.y, z - sub.z);
#endif
    }

	inline Point3f operator*(const float& s) const {
#ifdef __ENABLE_SSE_POINT__
    __m128 SSEa = SSE_operand;
    __m128 SSEb = _mm_load1_ps((float*)&s);

    __declspec(align(16)) Point3f rPoint3f;

    rPoint3f.SSE_operand = _mm_mul_ps(SSEa,SSEb);

    return rPoint3f;
#else
    return Point3f(x * s, y * s, z * s);
#endif
    }

    Point3f operator+(const Vector3f& add) const;

};

struct Vector3f {
    union {
        struct {
            float x;
            float y;
            float z;
            float pad;
        };
        __m128 SSE_operand;
    };

    Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
    Vector3f() : x(0), y(0), z(0), pad(0) { }

    inline Vector3f(const Point3f& start, const Point3f& end) {
#ifdef __ENABLE_DIV_VECTOR__
	SSE_operand = _mm_sub_ps(end.SSE_operand, start.SSE_operand);
#else
    x = end.x - start.x;
    y = end.y - start.y;
    z = end.z - start.z;
#endif
    }

    inline void Normalize() {
#ifdef __ENABLE_SSE_VECTOR__
    __declspec(align(16)) Vector3f Vector3fTmp;
    Vector3fTmp.SSE_operand = _mm_mul_ps(SSE_operand, SSE_operand);
    __declspec(align(16)) float length = sqrt(Vector3fTmp.x + Vector3fTmp.y + Vector3fTmp.z);
#else
    float length = sqrt(x * x + y * y + z * z);
#endif
	if (length != 0) {
		x /= length;
		y /= length;
		z /= length;
	}
    }

	inline Vector3f operator*(const float& s) const{
#ifdef __ENABLE_SSE_VECTOR__
    __m128 SSEb = _mm_load1_ps((float*)&s);

    __declspec(align(16)) Vector3f rVector3f;

    rVector3f.SSE_operand = _mm_mul_ps(SSE_operand, SSEb);

    return rVector3f;
#else
    return Vector3f(x * s, y * s, z * s);
#endif
    }

    inline Vector3f operator+(const Vector3f& add) const {
#ifdef __ENABLE_SSE_VECTOR__
    __m128 SSEa = SSE_operand;
    __m128 SSEb = add.SSE_operand;

    __declspec(align(16)) Vector3f rVector3f;

    rVector3f.SSE_operand = _mm_add_ps(SSEa, SSEb);

    return rVector3f;
#else
    return Vector3f(x + add.x, y + add.y, z + add.z);
#endif
    }

    inline Vector3f operator-(const Vector3f& sub) const {
#ifdef __ENABLE_SSE_VECTOR__
    __m128 SSEa = SSE_operand;
    __m128 SSEb = sub.SSE_operand;

    __declspec(align(16)) Vector3f rVector3f;

    rVector3f.SSE_operand = _mm_sub_ps(SSEa, SSEb);

    return rVector3f;
#else
    return Vector3f(x - sub.x, y - sub.y, z - sub.z);
#endif
    }

    inline float& operator [](int i) {
        if (i == 0)
            return x;
        else if (i == 1)
            return y;
        else
            return z;
    }

    inline float operator [](int i) const {
        if (i == 0)
            return x;
        else if (i == 1)
            return y;
        else
            return z;
    }
};

inline Point3f Point3f::operator+(const Vector3f& add) const {
#ifdef __ENABLE_SSE_POINT__
    __m128 SSEa = SSE_operand;
    __m128 SSEb = add.SSE_operand;

    __declspec(align(16)) Point3f rPoint3f;

    rPoint3f.SSE_operand = _mm_add_ps(SSEa, SSEb);

    return rPoint3f;
#else
    return Point3f(x + add.x, y + add.y, z + add.z);
#endif
}

// A 3x3 matrix. By default it is an identity matrix.
struct Matrix3x3 {
	__declspec(align(16)) float matrix[4][4];
    
	Matrix3x3() {
        make_identity();
    }

    void make_identity() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (i == j)
                    matrix[i][j] = 1.0;
                else
                    matrix[i][j] = 0.0;
            }
        }
    }

    Matrix3x3 operator*(const Matrix3x3& m) const {
        Matrix3x3 result;

        result[0][0] = matrix[0][0] * m[0][0] + matrix[0][1] * m[1][0] + matrix[0][2] * m[2][0];
        result[0][1] = matrix[0][0] * m[0][1] + matrix[0][1] * m[1][1] + matrix[0][2] * m[2][1];
        result[0][2] = matrix[0][0] * m[0][2] + matrix[0][1] * m[1][2] + matrix[0][2] * m[2][2];

        result[1][0] = matrix[1][0] * m[0][0] + matrix[1][1] * m[1][0] + matrix[1][2] * m[2][0];
        result[1][1] = matrix[1][0] * m[0][1] + matrix[1][1] * m[1][1] + matrix[1][2] * m[2][1];
        result[1][2] = matrix[1][0] * m[0][2] + matrix[1][1] * m[1][2] + matrix[1][2] * m[2][2];

        result[2][0] = matrix[2][0] * m[0][0] + matrix[2][1] * m[1][0] + matrix[2][2] * m[2][0];
        result[2][1] = matrix[2][0] * m[0][1] + matrix[2][1] * m[1][1] + matrix[2][2] * m[2][1];
        result[2][2] = matrix[2][0] * m[0][2] + matrix[2][1] * m[1][2] + matrix[2][2] * m[2][2];

        return result;
    }

    inline Vector3f operator*(const Vector3f& v) const {
        __declspec(align(16)) Vector3f result;

        result[0] = matrix[0][0] * v[0] + matrix[0][1] * v[1] + matrix[0][2] * v[2];
        result[1] = matrix[1][0] * v[0] + matrix[1][1] * v[1] + matrix[1][2] * v[2];
        result[2] = matrix[2][0] * v[0] + matrix[2][1] * v[1] + matrix[2][2] * v[2];

        return result;
    }

    float getDeterminant() const {
        return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
               matrix[1][0] * (matrix[0][1] * matrix[2][2] - matrix[2][1] * matrix[0][2]) +
               matrix[2][0] * (matrix[0][1] * matrix[1][2] - matrix[1][1] * matrix[0][2]);
    }

    float* operator [](int i) {
        return matrix[i];
    }

    const float* operator [](int i) const {
        return matrix[i];
    }
};

struct Ray {
    __declspec(align(16)) Point3f origin;
    __declspec(align(16)) Vector3f direction;
    float refrIndex;

    Ray(const Point3f& _o, const Vector3f& _d, float _IoF = 1): refrIndex(_IoF) {
        origin = _o;
        direction = _d;
        direction.Normalize();
    }

    RGBColor traceForColor(const Surface& surface, int depth, int diffuse_depth) const;

    RGBColor traceForLight(const Surface& surface, const Surface *light) const;
};

// Randomly pick a float between -r and +r.
inline float frand_radius(float r) {
    return (-r) + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (2.0f * r));
}

float _rand();

double get_wall_time();
double get_cpu_time();

inline float dot(const Vector3f& v1, const Vector3f& v2);
inline Vector3f cross(const Vector3f& v1, const Vector3f& v2);

std::shared_ptr<Surface> LoadMesh(const char *file_name, float scale, const Vector3f& offset);

void sse_test(int img_w, int img_h, int tracing_scene, int effort);

#endif