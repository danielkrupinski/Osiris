#pragma once

#include "AnimState.h"
#include "ClientClass.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "Cvar.h"
#include "Engine.h"
#include "EngineTrace.h"
#include "EntityList.h"
#include "Utils.h"
#include "Vector.h"
#include "WeaponId.h"
#include "VarMapping.h"
#include "../Memory.h"
#include "ModelRender.h"
#include "../SDK/matrix3x4.h"

struct AnimState;
struct WeaponData;

enum class MoveType {
    NOCLIP = 8,
    LADDER = 9
};

class Collideable {
public:
    virtual void* pad() = 0;
    virtual const Vector& obbMins() = 0;
    virtual const Vector& obbMaxs() = 0;
};

class Entity {
public:
    constexpr auto getCollideable() noexcept
    {
        return callVirtualMethod<Collideable*>(this, 3);
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

    constexpr bool setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) noexcept
    {
        if (config.misc.fixBoneMatrix) {
            int* render = reinterpret_cast<int*>(this + 0x274);
            int backup = *render;
            Vector absOrigin = getAbsOrigin();
            *render = 0;
            memory.setAbsOrigin(this, origin());
            auto result = callVirtualMethod<bool, matrix3x4*, int, int, float>(this + 4, 13, out, maxBones, boneMask, currentTime);
            memory.setAbsOrigin(this, absOrigin);
            *render = backup;
            return result;
        }
        return callVirtualMethod<bool, matrix3x4*, int, int, float>(this + 4, 13, out, maxBones, boneMask, currentTime);
    }

    constexpr auto getModel() noexcept
    {
        return callVirtualMethod<const Model*>(this + 4, 8);
    }

    Vector getBonePosition(int bone) noexcept
    {
        if (matrix3x4 boneMatrices[128]; setupBones(boneMatrices, 128, 256, 0.0f))
            return Vector{ boneMatrices[bone][0][3], boneMatrices[bone][1][3], boneMatrices[bone][2][3] };
        else
            return Vector{ };
    }

    auto getEyePosition() noexcept
    {
        Vector vec;
        callVirtualMethod<void, Vector&>(this, 284, vec);
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
        return memory.isOtherEnemy(this, interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()));
    }

    constexpr bool isDormant() noexcept
    {
        return callVirtualMethod<bool>(this + 8, 9);
    }

    constexpr void preDataUpdate(int updateType) noexcept
    {
        callVirtualMethod<void, int>(this + 8, 6, updateType);
    }

    constexpr void release() noexcept
    {
        return callVirtualMethod<void>(this + 8, 1);
    }

    constexpr void setDestroyedOnRecreateEntities() noexcept
    {
        return callVirtualMethod<void>(this + 8, 13);
    }

    constexpr bool isWeapon() noexcept
    {
        return callVirtualMethod<bool>(this, 165);
    }

    constexpr ClientClass* getClientClass() noexcept
    {
        return callVirtualMethod<ClientClass*>(this + 8, 2);
    }

    constexpr bool isAlive() noexcept
    {
        return callVirtualMethod<bool>(this, 155) && health() > 0;
    }

    constexpr bool isPlayer() noexcept
    {
        return callVirtualMethod<bool>(this, 157);
    }

    constexpr Entity* getActiveWeapon() noexcept
    {
        return callVirtualMethod<Entity*>(this, 267);
    }

    constexpr int getWeaponSubType() noexcept
    {
        return callVirtualMethod<int>(this, 281);
    }

    constexpr WeaponData* getWeaponData() noexcept
    {
        return callVirtualMethod<WeaponData*>(this, 459);
    }

    constexpr float getInaccuracy() noexcept
    {
        return callVirtualMethod<float>(this, 481);
    }

    VarMap* getVarMap() noexcept
    {
        return reinterpret_cast<VarMap*>(this + 0x24);
    }
    
    constexpr Vector getAbsOrigin() noexcept
    {
        return callVirtualMethod<Vector&>(this, 10);
    }

    constexpr void setModelIndex(int index) noexcept
    {
        callVirtualMethod<void, int>(this, 75, index);
    }

    AnimState* getAnimstate() noexcept
    {
        return *reinterpret_cast<AnimState**>(this + 0x3900);
    }

    float getMaxDesyncAngle() noexcept
    {
        if (auto animState = getAnimstate()) {
            float yawModifier = (animState->stopToFullRunningFraction * -0.3f - 0.2f) * std::clamp(animState->footSpeed, 0.0f, 1.0f) + 1.0f;

            if (animState->duckAmount > 0.0f)
                yawModifier += (animState->duckAmount * std::clamp(animState->footSpeed2, 0.0f, 1.0f) * (0.5f - yawModifier));

            return animState->velocitySubtractY * yawModifier;
        }
        return 0.0f;
    }

    constexpr Entity* getObserverTarget() noexcept
    {
        return callVirtualMethod<Entity*>(this, 294);
    }

    bool isInReload() noexcept
    {
        return *reinterpret_cast<bool*>(uintptr_t(&clip()) + 0x41);
    }

    matrix3x4& coordinateFrame() noexcept
    {
        return *reinterpret_cast<matrix3x4*>(this + 0x444);
    }

    NETVAR(body, "CBaseAnimating", "m_nBody", int)
    NETVAR(hitboxSet, "CBaseAnimating", "m_nHitboxSet", int)

    NETVAR_OFFSET(index, "CBaseEntity", "m_bIsAutoaimTarget", 4, int)
    NETVAR(modelIndex, "CBaseEntity", "m_nModelIndex", unsigned)
    NETVAR(origin, "CBaseEntity", "m_vecOrigin", Vector)
    NETVAR_OFFSET(moveType, "CBaseEntity", "m_nRenderMode", 1, MoveType)
    NETVAR(simulationTime, "CBaseEntity", "m_flSimulationTime", float)
    NETVAR(ownerEntity, "CBaseEntity", "m_hOwnerEntity", int)
    NETVAR(team, "CBaseEntity", "m_iTeamNum", int)

    NETVAR(weapons, "CBaseCombatCharacter", "m_hMyWeapons", int[48])
    PNETVAR(wearables, "CBaseCombatCharacter", "m_hMyWearables", int)

    NETVAR(viewModel, "CBasePlayer", "m_hViewModel[0]", int)
    NETVAR(health, "CBasePlayer", "m_iHealth", int)
    NETVAR(fov, "CBasePlayer", "m_iFOV", int)
    NETVAR(fovStart, "CBasePlayer", "m_iFOVStart", int)
    NETVAR(flags, "CBasePlayer", "m_fFlags", int)
    NETVAR(tickBase, "CBasePlayer", "m_nTickBase", int)
    NETVAR(aimPunchAngle, "CBasePlayer", "m_aimPunchAngle", Vector)
    NETVAR(viewPunchAngle, "CBasePlayer", "m_viewPunchAngle", Vector)
    NETVAR(velocity, "CBasePlayer", "m_vecVelocity[0]", Vector)
    
    NETVAR(armor, "CCSPlayer", "m_ArmorValue", int)
    NETVAR(eyeAngles, "CCSPlayer", "m_angEyeAngles", Vector)
    NETVAR(isScoped, "CCSPlayer", "m_bIsScoped", bool)
    NETVAR(isDefusing, "CCSPlayer", "m_bIsDefusing", bool)
    NETVAR(flashDuration, "CCSPlayer", "m_flFlashDuration", float)
    NETVAR(flashMaxAlpha, "CCSPlayer", "m_flFlashMaxAlpha", float)
    NETVAR(gunGameImmunity, "CCSPlayer", "m_bGunGameImmunity", bool)
    NETVAR(account, "CCSPlayer", "m_iAccount", int)
    NETVAR(inBombZone, "CCSPlayer", "m_bInBombZone", bool)
    NETVAR(hasDefuser, "CCSPlayer", "m_bHasDefuser", bool)
    NETVAR(hasHelmet, "CCSPlayer", "m_bHasHelmet", bool)
    NETVAR(lby, "CCSPlayer", "m_flLowerBodyYawTarget", float)
    NETVAR(ragdoll, "CCSPlayer", "m_hRagdoll", int)

    NETVAR(viewModelIndex, "CBaseCombatWeapon", "m_iViewModelIndex", int)
    NETVAR(worldModelIndex, "CBaseCombatWeapon", "m_iWorldModelIndex", int)
    NETVAR(worldDroppedModelIndex, "CBaseCombatWeapon", "m_iWorldDroppedModelIndex", int)
    NETVAR(weaponWorldModel, "CBaseCombatWeapon", "m_hWeaponWorldModel", int)
    NETVAR(clip, "CBaseCombatWeapon", "m_iClip1", int)
    NETVAR(nextPrimaryAttack, "CBaseCombatWeapon", "m_flNextPrimaryAttack", float)

    NETVAR(nextAttack, "CBaseCombatCharacter", "m_flNextAttack", float)

    NETVAR(accountID, "CBaseAttributableItem", "m_iAccountID", int)
    NETVAR(itemDefinitionIndex, "CBaseAttributableItem", "m_iItemDefinitionIndex", short)
    NETVAR(itemDefinitionIndex2, "CBaseAttributableItem", "m_iItemDefinitionIndex", WeaponId)
    NETVAR(itemIDHigh, "CBaseAttributableItem", "m_iItemIDHigh", int)
    NETVAR(entityQuality, "CBaseAttributableItem", "m_iEntityQuality", int)
    NETVAR(customName, "CBaseAttributableItem", "m_szCustomName", char[32])
    NETVAR(fallbackPaintKit, "CBaseAttributableItem", "m_nFallbackPaintKit", unsigned)
    NETVAR(fallbackSeed, "CBaseAttributableItem", "m_nFallbackSeed", unsigned)
    NETVAR(fallbackWear, "CBaseAttributableItem", "m_flFallbackWear", float)
    NETVAR(fallbackStatTrak, "CBaseAttributableItem", "m_nFallbackStatTrak", unsigned)
    NETVAR(initialized, "CBaseAttributableItem", "m_bInitialized", bool)

    NETVAR(owner, "CBaseViewModel", "m_hOwner", int)
    NETVAR(weapon, "CBaseViewModel", "m_hWeapon", int)

    NETVAR(c4StartedArming, "CC4", "m_bStartedArming", bool)

    NETVAR(c4BlowTime, "CPlantedC4", "m_flC4Blow", float)
    NETVAR(c4BombSite, "CPlantedC4", "m_nBombSite", int)
    NETVAR(c4Ticking, "CPlantedC4", "m_bBombTicking", bool)
    NETVAR(c4DefuseCountDown, "CPlantedC4", "m_flDefuseCountDown", float)
    NETVAR(c4Defuser, "CPlantedC4", "m_hBombDefuser", int)

    NETVAR(tabletReceptionIsBlocked, "CTablet", "m_bTabletReceptionIsBlocked", bool)
    
    NETVAR(droneTarget, "CDrone", "m_hMoveToThisEntity", int)

    NETVAR(sentryHealth, "CDronegun", "m_iHealth", int)
};
