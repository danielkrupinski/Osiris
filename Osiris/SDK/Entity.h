#pragma once

#include "ClientClass.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "Utils.h"

class Entity {
public:
    int* getFlags() noexcept
    {
        return reinterpret_cast<int*>(this + netvars.getOffset("m_fFlags"));
    }

    int getMoveType() noexcept
    {
        return *reinterpret_cast<int*>(this + netvars.getOffset("m_MoveType"));
    }

    bool isScoped() noexcept
    {
        return *reinterpret_cast<bool*>(this + netvars.getOffset("m_bIsScoped"));
    }

    int getCrosshairID() noexcept
    {
        return *reinterpret_cast<int*>(this + netvars.getOffset("m_bHasDefuser") + 92);
    }

    constexpr void drawModel(int flags, unsigned char alpha) noexcept
    {
        return callVirtualFunction<void(__thiscall*)(void*, int, unsigned char)>(this + 4, 9)(this + 4, flags, alpha);
    }

    bool isEnemy() noexcept
    {
        static auto gameMode = interfaces.cvar->findVar("game_type");

        if (!isInLocalTeam() || gameMode->getInt() == 6)
            return true;
        else
            return false;
    }

    constexpr bool isDormant() noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this + 8, 9)(this + 8);
    }

    constexpr bool isWeapon() noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 161)(this);
    }

    constexpr ClientClass* getClientClass() noexcept
    {
        return callVirtualFunction<ClientClass*(__thiscall*)(void*)>(this + 8, 2)(this + 8);
    }

    constexpr bool isAlive() noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 151)(this);
    }

    constexpr bool isInLocalTeam() noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 92)(this);
    }
};
