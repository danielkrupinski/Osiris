#pragma once

#include "Utils.h"

class GameEvent {
public:
    constexpr auto getName() noexcept
    {
        return callVirtualMethod<const char*>(this, 1);
    }
};
