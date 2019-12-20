#pragma once

struct Vector;

class Angle {
    float sinPitch{ 0.0f }, cosPitch{ 0.0f };
    float sinYaw{ 0.0f }, cosYaw{ 0.0f };
    float sinRoll{ 0.0f }, cosRoll{ 0.0f };

public:
    Vector forward;
    Vector right;
    Vector up;

    const Angle(const Vector& v) noexcept :sinPitch{ std::sin(degreesToRadians(v.x)) }, cosPitch{ std::cos(degreesToRadians(v.x)) }, sinYaw{ std::sin(degreesToRadians(v.y)) },
                                    cosYaw{ std::cos(degreesToRadians(v.y)) }, sinRoll{ std::sin(degreesToRadians(v.z)) }, cosRoll{ std::cos(degreesToRadians(v.z)) }
    {
        forward = { cosPitch * cosYaw, cosPitch * sinYaw, -sinPitch };
        right = { (-1 * sinRoll * sinPitch * cosYaw + -1 * cosRoll * -sinYaw), (-1 * sinRoll * sinPitch * sinYaw + -1 * cosRoll * cosYaw), -1 * sinRoll * cosPitch };
        up = { (cosRoll * sinPitch * cosYaw + -sinRoll * -sinYaw), (cosRoll * sinPitch * sinYaw + -sinRoll * cosYaw), cosRoll * cosPitch };
    }
};
