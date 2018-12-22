#pragma once

#include "Utils.h"

class EntityList {
public:
    constexpr class Entity* getClientEntity(int index) noexcept
    {
        return callVirtualFunction<class Entity*(__thiscall*)(void*, int)>(this, 3)(this, index);
    }

    constexpr int getHighestEntityIndex() noexcept
    {
        return callVirtualFunction<int(__thiscall*)(void*)>(this, 6)(this);
    }
};
