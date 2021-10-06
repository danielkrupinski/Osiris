#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class Surface {
public:
    INCONSTRUCTIBLE(Surface)

    VIRTUAL_METHOD(void, unlockCursor, 66, (), (this))
};
