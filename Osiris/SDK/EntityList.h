#pragma once

#include "Entity.h"
#include "Utils.h"

class ClientEntityList {
public:
    constexpr Entity* getClientEntity(int index) noexcept
    {
        return callVirtualFunction<Entity*(__thiscall*)(void*, int)>(this, 3)(this, index);
    }

    constexpr int getHighestEntityIndex() noexcept
    {
        return callVirtualFunction<int(__thiscall*)(void*)>(this, 6)(this);
    }
};
