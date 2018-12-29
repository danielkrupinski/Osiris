#pragma once

struct QAngle final {
    constexpr QAngle& operator-(const QAngle& v) noexcept
    {
        pitch -= v.pitch;
        yaw -= v.yaw;
        roll -= v.roll;
        return *this;
    }

    float pitch;
    float yaw;
    float roll;
};
