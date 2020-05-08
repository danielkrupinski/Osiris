#pragma once

#include "Utils.h"

class Entity;

class EntityList {
public:
    constexpr auto getEntity(int index) noexcept
    {
        return callVirtualMethod<Entity*, int>(this, 3, index);
    }

    constexpr auto getEntityFromHandle(int handle) noexcept
    {
        return callVirtualMethod<Entity*, int>(this, 4, handle);
    }

    constexpr auto getHighestEntityIndex() noexcept
    {
        return callVirtualMethod<int>(this, 6);
    }
};
