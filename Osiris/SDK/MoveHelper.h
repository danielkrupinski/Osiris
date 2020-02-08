#pragma once

#include "Utils.h"

class Entity;

class MoveHelper {
public:
    constexpr void setHost(Entity* host) noexcept
    {
        return callVirtualMethod<void>(this, 1, host);
    }
};
