#pragma once
#include <cmath>

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2 operator+(const Vec2& other) const {
        return Vec2{x + other.x, y + other.y};
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2{x - other.x, y - other.y};
    }

    Vec2 operator*(float scalar) const {
        return Vec2{x * scalar, y * scalar};
    }

    Vec2 operator/(float scalar) const {
        return Vec2{x / scalar, y / scalar};
    }

    Vec2 &operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2 &operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2 &operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2 &operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    float length() const {
        return std::sqrt(lengthSquared());
    }

    float lengthSquared() const {
        return x * x + y * y;
    }

};

inline Vec2 normalize(const Vec2& v) {
    float len = v.length();
    if (len <= 1e-6f) {
        return Vec2{0.0f, 0.0f};
    }
    return v / len;
}

inline Vec2 clampMag(const Vec2& v, float maxLen) {
    float L2 = v.lengthSquared();
    if (L2 <= maxLen*maxLen) return v;
    float L = std::sqrt(L2);
    return v * (maxLen / L);
}