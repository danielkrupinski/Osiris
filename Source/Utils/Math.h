#pragma once

#include <xmmintrin.h>

namespace Math {
    inline float abs(float x) noexcept
    {
        return x < 0.0f ? -x : x;
    }

    inline void sincos(float x, float& sine, float& cosine) noexcept
    {
        constexpr float kPi = 3.14159265358979323846f;
        constexpr float kHalfPi = kPi * 0.5f;
        constexpr float kTwoPi = kPi * 2.0f;
        constexpr float kInvTwoPi = 1.0f / kTwoPi;

        // Reduce first to [-pi, pi], then to [-pi/2, pi/2]. The polynomials
        // only operate on this small interval, including yaw at +/-180 degrees.
        x -= static_cast<float>(static_cast<int>(x * kInvTwoPi)) * kTwoPi;
        if (x > kPi)
            x -= kTwoPi;
        else if (x < -kPi)
            x += kTwoPi;

        float cosineSign = 1.0f;
        if (x > kHalfPi) {
            x = kPi - x;
            cosineSign = -1.0f;
        } else if (x < -kHalfPi) {
            x = -kPi - x;
            cosineSign = -1.0f;
        }

        float x2 = x * x;
        // Taylor terms through x^11/x^10 have sub-micro-unit error here.
        sine = x * (1.0f + x2 * (-1.0f / 6.0f + x2 * (1.0f / 120.0f + x2 * (-1.0f / 5040.0f + x2 * (1.0f / 362880.0f - x2 / 39916800.0f)))));
        cosine = cosineSign * (1.0f + x2 * (-0.5f + x2 * (1.0f / 24.0f + x2 * (-1.0f / 720.0f + x2 * (1.0f / 40320.0f - x2 / 3628800.0f)))));
    }

}
