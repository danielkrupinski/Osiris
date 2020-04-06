#pragma once

#include <cassert>

class Entity;

class LocalPlayer {
public:
    void init(Entity** entity) noexcept
    {
        assert(!localEntity);
        localEntity = entity;
    }

    constexpr operator bool() noexcept
    {
        assert(localEntity);
        return *localEntity != nullptr;
    }

    constexpr auto operator->() noexcept
    {
        assert(localEntity && *localEntity);
        return *localEntity;
    }

    constexpr auto get() noexcept
    {
        assert(localEntity && *localEntity);
        return *localEntity;
    }
private:
    Entity** localEntity = nullptr;
};

inline LocalPlayer localPlayer;
