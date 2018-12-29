#pragma once

#include "ClientClass.h"
#include "../Interfaces.h"
#include "Matrix3x4.h"
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

    constexpr void drawModel(int flags, unsigned char alpha) noexcept
    {
        return callVirtualFunction<void(__thiscall*)(void*, int, unsigned char)>(this + 4, 9)(this + 4, flags, alpha);
    }

    constexpr bool setupBones(Matrix3x4* out, int maxBones, int boneMask, float currentTime) noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*, Matrix3x4*, int, int, float)>(this + 4, 13)(this + 4, out, maxBones, boneMask, currentTime);
    }

    Vector getBonePosition(int bone)
    {
        Matrix3x4 boneMatrixes[128];
        if (setupBones(boneMatrixes, 128, 256, 0.0f)) {
            Matrix3x4 boneMatrix = boneMatrixes[bone];
            return Vector{ boneMatrix.matrix[0][3], boneMatrix.matrix[1][3], boneMatrix.matrix[2][3] };
        }
        else
            return Vector{ };
    }

    Vector getEyePosition()
    {
        return *reinterpret_cast<Vector*>(this + netvars.getOffset("m_vecOrigin")) + *reinterpret_cast<Vector*>(this + netvars.getOffset("m_vecViewOffset[0]"));
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
