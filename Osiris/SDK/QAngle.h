#pragma once

#include <cmath>

struct QAngle final {
    constexpr QAngle& operator-(const QAngle& v) noexcept
    {
        pitch -= v.pitch;
        yaw -= v.yaw;
        roll -= v.roll;
        return *this;
    }

    constexpr auto operator-(const QAngle& v) const noexcept
    {
        return QAngle{ pitch - v.pitch, yaw - v.yaw, roll - v.roll };
    }

    constexpr QAngle& operator/(float div) noexcept
    {
        pitch /= div;
        yaw /= div;
        roll /= div;
        return *this;
    }

    constexpr QAngle& operator+=(const QAngle& v) noexcept
    {
        pitch += v.pitch;
        yaw += v.yaw;
        roll += v.roll;
        return *this;
    }

    void normalize() noexcept
    {
        pitch = std::isfinite(pitch) ? std::remainderf(pitch, 360.0f) : 0.0f;
        yaw = std::isfinite(yaw) ? std::remainderf(yaw, 360.0f) : 0.0f;
        roll = 0.0f;
    }

    float pitch;
    float yaw;
    float roll;
};
