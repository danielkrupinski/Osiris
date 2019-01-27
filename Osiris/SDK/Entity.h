#pragma once

#include "ClientClass.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "Utils.h"
#include "Vector.h"

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

    bool isImmune() noexcept
    {
        return *reinterpret_cast<bool*>(this + netvars.getOffset("m_bGunGameImmunity"));
    }

    int getActiveWeaponHandle() noexcept
    {
        return *reinterpret_cast<int*>(this + netvars.getOffset("m_hActiveWeapon"));
    }

    float getNextPrimaryAttack() noexcept
    {
        return *reinterpret_cast<float*>(this + netvars.getOffset("m_flNextPrimaryAttack"));
    }

    short getItemDefinitionIndex() noexcept
    {
        return *reinterpret_cast<short*>(this + netvars.getOffset("m_iItemDefinitionIndex"));
    }

    constexpr bool isPistol() noexcept
    {
        switch (getClientClass()->classId) {
        case ClassId::Deagle:
        case ClassId::Elite:
        case ClassId::FiveSeven:
        case ClassId::Glock:
        case ClassId::P2000:
        case ClassId::P250:
        case ClassId::Tec9:
            return true;
        default:
            return false;
        }
    }
    constexpr void drawModel(int flags, unsigned char alpha) noexcept
    {
        return callVirtualFunction<void(__thiscall*)(void*, int, unsigned char)>(this + 4, 9)(this + 4, flags, alpha);
    }

    using matrix3x4 = float[3][4];

    constexpr bool setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*, matrix3x4*, int, int, float)>(this + 4, 13)(this + 4, out, maxBones, boneMask, currentTime);
    }

    Vector getBonePosition(int bone)
    {
        matrix3x4 boneMatrices[128];
        if (setupBones(boneMatrices, 128, 256, 0.0f))
            return Vector{ boneMatrices[bone][0][3], boneMatrices[bone][1][3], boneMatrices[bone][2][3] };
        else
            return Vector{ };
    }

    Vector getEyeOrigin()
    {
        Vector vec;
        callVirtualFunction<void(__thiscall*)(void*, Vector&)>(this, 279)(this, vec);
        return vec;
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
