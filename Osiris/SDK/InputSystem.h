#pragma once

#include "Utils.h"

class InputSystem {
public:
    void enableInput(bool enable)
    {
        callVirtualFunction<void(__thiscall*)(void*, bool)>(this, 11)(this, enable);
    }

    void resetInputState()
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 39)(this);
    }
};
