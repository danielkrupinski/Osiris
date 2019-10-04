#pragma once
#include "Vector.h"

class Angle {
    float sp{ 0.f }, cp{ 0.f };
    float sy{ 0.f }, cy{ 0.f };
    float sr{ 0.f }, cr{ 0.f };

    Vector get(unsigned int i) noexcept {
        switch (i) {
        case 0:
            return Vector{ cp * cy, cp * sy, -sp };
            break;
        case 1:
            return Vector{ (-1 * sr * sp * cy + -1 * cr * -sy), (-1 * sr * sp * sy + -1 * cr * cy), (-1 * sr * cp) };
            break;
        case 2:
            return Vector{ (cr * sp * cy + -sr * -sy), (cr * sp * sy + -sr * cy), (cr * cp) };
            break;
        default:
            return Vector{ };
            break;
        }
    }

public:
    Vector forward{ };
    Vector right{ };
    Vector up{ };

    Angle(const Vector& v) noexcept :sp{ sinf(v.x) }, cp{ cosf(v.x) }, sy{ sinf(v.y) }, cy{ cosf(v.y) }, sr{ sinf(v.z) }, cr{ cosf(v.z) },
                                    forward{ get(0) }, right{ get(1) }, up{ get(2) } { }
};
