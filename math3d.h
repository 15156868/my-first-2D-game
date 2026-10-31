#pragma once
#include <cmath>

inline float radians(float deg) { return deg * 3.1415926535f / 180.0f; }

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& o) const { return Vec3(x + o.x, y + o.y, z + o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
};

inline float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
inline float length(const Vec3& v) { return std::sqrt(dot(v, v)); }
inline Vec3 normalize(const Vec3& v) {
    float l = length(v);
    return l > 0.0f ? v * (1.0f / l) : v;
}

struct Mat4 {
    float m[16];
    Mat4() { for (int i = 0; i < 16; ++i) m[i] = 0.0f; }
    static Mat4 identity() {
        Mat4 r;
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }
};

inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 r;
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k)
                sum += a.m[k * 4 + row] * b.m[col * 4 + k];
            r.m[col * 4 + row] = sum;
        }
    return r;
}

inline Mat4 translate(const Mat4& mat, const Vec3& t) {
    Mat4 r = mat;
    r.m[12] = mat.m[0] * t.x + mat.m[4] * t.y + mat.m[8] * t.z + mat.m[12];
    r.m[13] = mat.m[1] * t.x + mat.m[5] * t.y + mat.m[9] * t.z + mat.m[13];
    r.m[14] = mat.m[2] * t.x + mat.m[6] * t.y + mat.m[10] * t.z + mat.m[14];
    r.m[15] = mat.m[3] * t.x + mat.m[7] * t.y + mat.m[11] * t.z + mat.m[15];
    return r;
}

inline Mat4 scale(const Mat4& mat, const Vec3& s) {
    Mat4 r = mat;
    for (int row = 0; row < 4; ++row) {
        r.m[0 * 4 + row] = mat.m[0 * 4 + row] * s.x;
        r.m[1 * 4 + row] = mat.m[1 * 4 + row] * s.y;
        r.m[2 * 4 + row] = mat.m[2 * 4 + row] * s.z;
    }
    return r;
}

inline Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = normalize(center - eye);
    Vec3 s = normalize(cross(f, up));
    Vec3 u = cross(s, f);
    Mat4 r = Mat4::identity();
    r.m[0] = s.x; r.m[4] = s.y; r.m[8] = s.z;
    r.m[1] = u.x; r.m[5] = u.y; r.m[9] = u.z;
    r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
    r.m[12] = -dot(s, eye);
    r.m[13] = -dot(u, eye);
    r.m[14] = dot(f, eye);
    return r;
}

inline Mat4 perspective(float fovyRad, float aspect, float znear, float zfar) {
    float t = std::tan(fovyRad * 0.5f);
    Mat4 r;
    r.m[0] = 1.0f / (aspect * t);
    r.m[5] = 1.0f / t;
    r.m[10] = -(zfar + znear) / (zfar - znear);
    r.m[11] = -1.0f;
    r.m[14] = -(2.0f * zfar * znear) / (zfar - znear);
    return r;
}

inline const float* valuePtr(const Mat4& m) { return m.m; }