#pragma once

#include <cassert>

#include "Entity.h"

class LocalPlayer {
public:
    void init(std::uintptr_t* entity) noexcept
    {
        assert(!localEntity);
        localEntity = entity;
    }

    constexpr operator bool() noexcept
    {
        assert(localEntity);
        return *localEntity != 0;
    }

    [[nodiscard]] auto get() noexcept
    {
        assert(localEntity && *localEntity);
        return Entity{ retSpoofGadgets.client, *localEntity };
    }

private:
    std::uintptr_t* localEntity = nullptr;
};

inline LocalPlayer localPlayer;
