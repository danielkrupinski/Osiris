#pragma once

#include "Utils.h"

class InputSystem {
    void enableInput(bool enable)
    {
        callVirtualFunction<void(__thiscall*)(void*, bool)>(this, 11)(this, enable);
    }
};
