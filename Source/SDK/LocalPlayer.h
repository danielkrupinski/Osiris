#pragma once

#include <cassert>

#include "Entity.h"
#include <RetSpoofGadgets.h>

class LocalPlayer {
public:
    void init(csgo::pod::Entity** entity) noexcept
    {
        assert(!localEntity);
        localEntity = entity;
    }

    constexpr operator bool() noexcept
    {
        assert(localEntity);
        return *localEntity != nullptr;
    }

    [[nodiscard]] auto get() noexcept
    {
        assert(localEntity && *localEntity);
        return Entity::from(retSpoofGadgets->client, *localEntity);
    }

private:
    csgo::pod::Entity** localEntity = nullptr;
};

inline LocalPlayer localPlayer;
