#pragma once

#include "Utils.h"
#include "Vector.h"

class GameMovement {
public:
    constexpr Vector getPlayerViewOffset(bool ducked) noexcept
    {
        return callVirtualMethod<Vector&, bool>(this, 8, ducked);
    }
};
