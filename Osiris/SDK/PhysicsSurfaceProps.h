#pragma once

#include <cstddef>

struct SurfaceData {
    std::byte pad[80];
    float maxspeedfactor;
    float jumpfactor;
    float penetrationmodifier;
    float damagemodifier;
    short material;
    bool climbable;
};
