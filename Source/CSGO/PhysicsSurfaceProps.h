#pragma once

#include "Pad.h"
#include "VirtualMethod.h"

namespace csgo
{

struct SurfaceData {
    PAD(80);
    float maxspeedfactor;
    float jumpfactor;
    float penetrationmodifier;
    float damagemodifier;
    short material;
    bool climbable;
};

struct PhysicsSurfacePropsPOD;

struct PhysicsSurfaceProps : VirtualCallableFromPOD<PhysicsSurfaceProps, PhysicsSurfacePropsPOD> {
    VIRTUAL_METHOD_V(SurfaceData*, getSurfaceData, 5, (int index), (index))
};

}
