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

    void*& getMutableWindowHandle()
    {
        static uint32_t offset = 0;
        if (!offset)
            offset = *(uint32_t*)((*(char***)this)[10] + 5);
        return *(void**)((char*)this + offset);
    }
};
