#pragma once

#include "Utils.h"

class InputSystem {
public:
    constexpr void enableInput(bool enable) noexcept
    {
        callVirtualFunction<void, bool>(this, 11)(this, enable);
    }

    constexpr void resetInputState() noexcept
    {
        callVirtualFunction<void>(this, 39)(this);
    }
};
