#pragma once

#include "Utils.h"

class InputSystem {
public:
    constexpr void enableInput(bool enable) noexcept
    {
        callVirtualMethod<void, bool>(this, 11, enable);
    }

    constexpr void resetInputState() noexcept
    {
        callVirtualMethod<void>(this, 39);
    }
};
