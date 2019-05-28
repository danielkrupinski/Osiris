#pragma once

#include "Utils.h"

class GameEventManager {
public:
    constexpr auto addListener(void* listener, const char* name) noexcept
    {
        return callVirtualMethod<bool, void*, const char*, bool>(this, 3, listener, name, false);
    }
};
