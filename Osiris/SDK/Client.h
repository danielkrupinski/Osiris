#pragma once

#include "Utils.h"

class Client final {
public:
    void activateMouse()
    {
        callVirtualFunction<void(__thiscall*)()>(this, 14)();
    }
};
