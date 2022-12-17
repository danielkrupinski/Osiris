#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct SurfacePOD;

class Surface : public VirtualCallableFromPOD<Surface, SurfacePOD> {
public:
    VIRTUAL_METHOD(void, unlockCursor, 66, (), ())
};

}
