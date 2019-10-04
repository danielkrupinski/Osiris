#pragma once

#include <cmath>
#include <cfloat>
#include <algorithm>

#include "matrix3x4.h"

struct Vector {
    constexpr operator bool() const noexcept
    {
        return x || y || z;
    }

    constexpr Vector& operator=(float array[3]) noexcept
    {
        x = array[0];
        y = array[1];
        z = array[2];
        return *this;
    }

    constexpr Vector& operator+=(const Vector& v) noexcept
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    constexpr Vector& operator-=(const Vector& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    constexpr auto operator-(const Vector& v) const noexcept
    {
        return Vector{ x - v.x, y - v.y, z - v.z };
    }

    constexpr auto operator+(const Vector& v) const noexcept
    {
        return Vector{ x + v.x, y + v.y, z + v.z };
    }

    constexpr Vector& operator/=(float div) noexcept
    {
        x /= div;
        y /= div;
        z /= div;
        return *this;
    }

    constexpr auto operator*(float mul) const noexcept
    {
        return Vector{ x * mul, y * mul, z * mul };
    }

    constexpr Vector& operator*=(float mul) noexcept
    {
        x *= mul;
        y *= mul;
        z *= mul;
        return *this;
    }

    constexpr void normalize() noexcept
    {
        x = std::isfinite(x) ? std::remainderf(x, 360.0f) : 0.0f;
        y = std::isfinite(y) ? std::remainderf(y, 360.0f) : 0.0f;
        z = 0.0f;
    }

    auto length() const noexcept
    {
        return std::hypot(x, y, z);
    }

    auto length2D() const noexcept
    {
        return std::hypotf(x, y);
    }

    constexpr auto squareLength() noexcept
    {
        return x * x + y * y + z * z;
    }

    auto distance(const Vector& v) const noexcept
    {
        return std::hypot(x - v.x, y - v.y, z - v.z);
    }

    constexpr auto clamp() noexcept
    {
        x = std::clamp(x, -89.0f, 89.0f);
        y = std::clamp(y, -180.0f, 180.0f);
        z = 0.0f;
    }

    void normalizeInPlace() noexcept
    {
        const float radius{ std::hypot(x, y, z) };

        // FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
        const float iradius{ 1.f / (radius + FLT_EPSILON) };

        x *= iradius;
        y *= iradius;
        z *= iradius;
    }

    constexpr auto dotProduct(const Vector& v) noexcept
    {
        return x * v.x + y * v.y + z * v.z;
    }

    constexpr auto transform(matrix3x4& mat) noexcept
    {
        return Vector{ dotProduct({ mat[0][0], mat[0][1], mat[0][2] }) + mat[0][3],
                       dotProduct({ mat[1][0], mat[1][1], mat[1][2] }) + mat[1][3],
                       dotProduct({ mat[2][0], mat[2][1], mat[2][2] }) + mat[2][3] };
    }

    float x, y, z;
};
