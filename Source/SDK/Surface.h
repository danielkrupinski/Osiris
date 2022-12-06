#pragma once

#include "VirtualMethod.h"

namespace csgo::pod { struct Surface; }

class Surface : public VirtualCallableFromPOD<Surface, csgo::pod::Surface> {
public:
    VIRTUAL_METHOD(void, unlockCursor, 66, (), ())
};
