#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>

enum class VecType : uint8_t {
    LOCATION,
    VELOCITY,
    UNKNOWN,
};

struct Vec2 {
    float x;
    float y;
    VecType type = VecType::UNKNOWN;

    Vec2 &operator=(const Vec2 &rhs)
    {
        if (this == &rhs) {
            return *this;
        }
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    Vec2 &operator+=(const Vec2 &rhs)
    {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    Vec2 &operator-=(const Vec2 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    Vec2 &operator*=(const Vec2 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;

        return *this;
    }

    Vec2 &operator*=(const float t)
    {
        x *= t;
        y *= t;

        return *this;
    }

    const Vec2 operator+(const Vec2 &other) const
    {
        Vec2 result = *this;
        result += other;

        return result;
    }

    const Vec2 operator-(const Vec2 &other) const
    {
        Vec2 result = *this;
        result -= other;

        return result;
    }

    const Vec2 operator*(const Vec2 &other) const
    {
        Vec2 result = *this;
        result *= other;

        return result;
    }

    const Vec2 operator*(const float t) const
    {
        Vec2 result = *this;
        result *= t;

        return result;
    }

    bool operator==(const Vec2 &other) const
    {
        return x == other.x && y == other.y;
    }

    float norm() const
    {
        return std::sqrt(x * x + y * y);
    }

    friend std::ostream &operator<<(std::ostream &os, const Vec2 &v);
};

inline std::ostream &operator<<(std::ostream &os, const Vec2 &v)
{
    os << v.x << ", " << v.y;
    return os;
}

struct Color {
    float r;
    float g;
    float b;
    float a;
};
