#pragma once

#include "ClientClass.h"
#include "../Interfaces.h"
#include "Utils.h"

class Entity {
public:
    int* getFlags()
    {
        return reinterpret_cast<int*>(this + 0x104);
    }

    int getMoveType()
    {
        return *reinterpret_cast<int*>(this + 0x25C);
    }

    bool isScoped()
    {
        return *reinterpret_cast<bool*>(this + 0x390A);
    }

    int getCrosshairID()
    {
        return *reinterpret_cast<int*>(this + 0xB394);
    }

    void drawModel(int flags, unsigned char alpha)
    {
        return callVirtualFunction<void(__thiscall*)(void*, int, unsigned char)>(this + 4, 9)(this + 4, flags, alpha);
    }

    bool isEnemy()
    {
        static auto gameMode = interfaces.cvar->findVar("game_type");

        if (!isInLocalTeam() || gameMode->getInt() == 6)
            return true;
        else
            return false;
    }

    bool isDormant()
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this + 8, 9)(this + 8);
    }

    bool isWeapon()
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 161)(this);
    }

    ClientClass* getClientClass()
    {
        return callVirtualFunction<ClientClass*(__thiscall*)(void*)>(this + 8, 2)(this + 8);
    }

    bool isAlive()
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 151)(this);
    }

    bool isInLocalTeam()
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 92)(this);
    }
};
