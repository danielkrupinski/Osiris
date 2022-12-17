#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct InputSystemPOD;

class InputSystem : public VirtualCallableFromPOD<InputSystem, InputSystemPOD> {
public:
    VIRTUAL_METHOD(void, enableInput, 11, (bool enable), (enable))
    VIRTUAL_METHOD(void, resetInputState, 39, (), ())
};

}
