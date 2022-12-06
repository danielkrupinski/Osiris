#pragma once

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

namespace csgo::pod { struct PhysicsSurfaceProps; }

class PhysicsSurfaceProps : public VirtualCallableFromPOD<PhysicsSurfaceProps, csgo::pod::PhysicsSurfaceProps> {
public:
    VIRTUAL_METHOD_V(SurfaceData*, getSurfaceData, 5, (int index), (index))
};
