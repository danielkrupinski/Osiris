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

class PhysicsSurfaceProps : public VirtualCallableFromPOD<PhysicsSurfaceProps, PhysicsSurfacePropsPOD> {
public:
    VIRTUAL_METHOD_V(SurfaceData*, getSurfaceData, 5, (int index), (index))
};

}
