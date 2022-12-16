#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

namespace csgo
{

namespace pod { struct InputSystem; }

class InputSystem : public VirtualCallableFromPOD<InputSystem, pod::InputSystem> {
public:
    VIRTUAL_METHOD(void, enableInput, 11, (bool enable), (enable))
    VIRTUAL_METHOD(void, resetInputState, 39, (), ())
};

}
