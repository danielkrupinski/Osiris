#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class MoveHelper {
public:
    INCONSTRUCTIBLE(MoveHelper)

    VIRTUAL_METHOD(void, setHost, 1, (std::uintptr_t host), (this, host))
};
