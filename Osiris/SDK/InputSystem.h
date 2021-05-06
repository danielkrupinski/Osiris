#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class InputSystem {
public:
    INCONSTRUCTIBLE(InputSystem)

    VIRTUAL_METHOD(void, enableInput, 11, (bool enable), (this, enable))
    VIRTUAL_METHOD(void, resetInputState, 39, (), (this))
};
