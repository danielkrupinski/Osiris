#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct InputSystemPOD;

struct InputSystem : GameClass<InputSystem, InputSystemPOD> {
    VIRTUAL_METHOD(void, enableInput, 11, (bool enable), (enable))
    VIRTUAL_METHOD(void, resetInputState, 39, (), ())
};

}
