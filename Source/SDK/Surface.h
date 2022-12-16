#pragma once

#include "VirtualMethod.h"

namespace csgo
{

namespace pod { struct Surface; }

class Surface : public VirtualCallableFromPOD<Surface, pod::Surface> {
public:
    VIRTUAL_METHOD(void, unlockCursor, 66, (), ())
};

}
