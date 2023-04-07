#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct SurfacePOD;

struct Surface : GameClass<Surface, SurfacePOD> {
    VIRTUAL_METHOD(void, unlockCursor, 66, (), ())
};

}
