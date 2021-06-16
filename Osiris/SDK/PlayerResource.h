#pragma once

#include "Inconstructible.h"
#include "../Netvars.h"
#include "VirtualMethod.h"

struct Vector;

class IPlayerResource {
public:
    INCONSTRUCTIBLE(IPlayerResource)

    VIRTUAL_METHOD_V(bool, isAlive, 5, (int index), (this, index))
    VIRTUAL_METHOD_V(const char*, getPlayerName, 8, (int index), (this, index))
    VIRTUAL_METHOD_V(int, getPlayerHealth, 14, (int index), (this, index))
};

class PlayerResource {
public:
    INCONSTRUCTIBLE(PlayerResource)

    auto getIPlayerResource() noexcept
    {
        return reinterpret_cast<IPlayerResource*>(std::uintptr_t(this) + WIN32_LINUX(0x9D8, 0xF68));
    }

    NETVAR(bombsiteCenterA, "CCSPlayerResource", "m_bombsiteCenterA", Vector)
    NETVAR(bombsiteCenterB, "CCSPlayerResource", "m_bombsiteCenterB", Vector)
    NETVAR(musicID, "CCSPlayerResource", "m_nMusicID", int[65])
    NETVAR(activeCoinRank, "CCSPlayerResource", "m_nActiveCoinRank", int[65])
};
