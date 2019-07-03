#pragma once

#include <cstddef>

#include "Utils.h"

struct SurfaceData {
    std::byte pad[80];
    float maxspeedfactor;
    float jumpfactor;
    float penetrationmodifier;
    float damagemodifier;
    short material;
    bool climbable;
};

class PhysicsSurfaceProps {
public:
    constexpr auto getSurfaceData(int index) noexcept
    {
        return callVirtualMethod<SurfaceData*, int>(this, 5, index);
    }
};
