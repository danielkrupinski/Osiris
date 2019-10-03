#pragma once

#include <cmath>

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

    float x, y, z;
};
