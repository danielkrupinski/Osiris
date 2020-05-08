#pragma once

#include "Utils.h"

class Entity;

class MoveHelper {
public:
    constexpr auto setHost(Entity* host) noexcept
    {
        callVirtualMethod<void>(this, 1, host);
    }
};
