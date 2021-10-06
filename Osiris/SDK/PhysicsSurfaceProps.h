#pragma once

#include "Inconstructible.h"
#include "Pad.h"
#include "VirtualMethod.h"

struct SurfaceData {
    PAD(80)
    float maxspeedfactor;
    float jumpfactor;
    float penetrationmodifier;
    float damagemodifier;
    short material;
    bool climbable;
};

class PhysicsSurfaceProps {
public:
    INCONSTRUCTIBLE(PhysicsSurfaceProps)

    VIRTUAL_METHOD_V(SurfaceData*, getSurfaceData, 5, (int index), (this, index))
};
