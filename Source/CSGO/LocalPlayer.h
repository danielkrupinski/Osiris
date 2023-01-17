#pragma once

#include <cassert>

#include "Entity.h"
#include <RetSpoof/RetSpoofGadgets.h>

class LocalPlayer {
public:
    void init(csgo::EntityPOD** entity) noexcept
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
        return csgo::Entity::from(retSpoofGadgets->client, *localEntity);
    }

private:
    csgo::EntityPOD** localEntity = nullptr;
};

inline LocalPlayer localPlayer;
