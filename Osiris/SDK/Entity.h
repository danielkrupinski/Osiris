#pragma once

#include "ClientClass.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "EngineTrace.h"
#include "Utils.h"
#include "Vector.h"
#include "WeaponId.h"

class Entity {
public:
    template <typename T>
    constexpr auto getProperty(const char* name, const std::size_t offset = 0) noexcept
    {
        return *reinterpret_cast<T*>(this + netvars[name] + offset);
    }

    template <typename T>
    constexpr void setProperty(const char* name, const T& value) noexcept
    {
        *reinterpret_cast<T*>(this + netvars[name]) = value;
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

    constexpr bool isSniperRifle() noexcept
    {
        switch (getClientClass()->classId) {
        case ClassId::Ssg08:
        case ClassId::Awp:
        case ClassId::Scar20:
        case ClassId::G3sg1:
            return true;
        default:
            return false;
        }
    }

    using matrix3x4 = float[3][4];

    constexpr bool setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*, matrix3x4*, int, int, float)>(this + 4, 13)(this + 4, out, maxBones, boneMask, currentTime);
    }

    Vector getBonePosition(int bone) noexcept
    {
        static matrix3x4 boneMatrices[128];
        if (setupBones(boneMatrices, 128, 256, 0.0f))
            return Vector{ boneMatrices[bone][0][3], boneMatrices[bone][1][3], boneMatrices[bone][2][3] };
        else
            return Vector{ };
    }

    Vector getEyePosition() noexcept
    {
        Vector vec;
        callVirtualFunction<void(__thiscall*)(void*, Vector&)>(this, 279)(this, vec);
        return vec;
    }

    bool isVisible(const Vector& position = { }) noexcept
    {
        auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        static Trace trace;
        interfaces.engineTrace->traceRay({ localPlayer->getEyePosition(), position ? position : getBonePosition(8) }, 0x46004009, { localPlayer }, trace);
        return trace.entity == this || trace.fraction > 0.97f;
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
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 151)(this) && getProperty<int>("m_iHealth") > 0;
    }

    constexpr bool isInLocalTeam() noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 92)(this);
    }

    constexpr float getInaccuracy() noexcept
    {
        return callVirtualFunction<float(__thiscall*)(void*)>(this, 471)(this);
    }
};
