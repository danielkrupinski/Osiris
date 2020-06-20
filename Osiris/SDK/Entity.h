#pragma once

#include "AnimState.h"
#include "ClientClass.h"
#include "Cvar.h"
#include "Engine.h"
#include "EngineTrace.h"
#include "EntityList.h"
#include "LocalPlayer.h"
#include "matrix3x4.h"
#include "ModelRender.h"
#include "Utils.h"
#include "VarMapping.h"
#include "Vector.h"
#include "VirtualMethod.h"
#include "WeaponData.h"
#include "WeaponId.h"

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"

#include <functional>

struct AnimState;

enum class MoveType {
    NOCLIP = 8,
    LADDER = 9
};

class Collideable {
public:
    VIRTUAL_METHOD(const Vector&, obbMins, 1, (), (this))
    VIRTUAL_METHOD(const Vector&, obbMaxs, 2, (), (this))
};

class Entity {
public:
    VIRTUAL_METHOD(void, release, 1, (), (this + 8))
    VIRTUAL_METHOD(ClientClass*, getClientClass, 2, (), (this + 8))
    VIRTUAL_METHOD(void, preDataUpdate, 6, (int updateType), (this + 8, updateType))
    VIRTUAL_METHOD(void, postDataUpdate, 7, (int updateType), (this + 8, updateType))
    VIRTUAL_METHOD(bool, isDormant, 9, (), (this + 8))
    VIRTUAL_METHOD(int, index, 10, (), (this + 8))
    VIRTUAL_METHOD(void, setDestroyedOnRecreateEntities, 13, (), (this + 8))

    VIRTUAL_METHOD(const Model*, getModel, 8, (), (this + 4))
    VIRTUAL_METHOD(const matrix3x4&, toWorldTransform, 32, (), (this + 4))

    VIRTUAL_METHOD(int&, handle, 2, (), (this))
    VIRTUAL_METHOD(Collideable*, getCollideable, 3, (), (this))
    VIRTUAL_METHOD(Vector&, getAbsOrigin, 10, (), (this))
    VIRTUAL_METHOD(void, setModelIndex, 75, (int index), (this, index))
    VIRTUAL_METHOD(int, health, 121, (), (this))
    VIRTUAL_METHOD(bool, isAlive, 155, (), (this))
    VIRTUAL_METHOD(bool, isPlayer, 157, (), (this))
    VIRTUAL_METHOD(bool, isWeapon, 165, (), (this))
    VIRTUAL_METHOD(Entity*, getActiveWeapon, 267, (), (this))
    VIRTUAL_METHOD(int, getWeaponSubType, 281, (), (this))
    VIRTUAL_METHOD(Entity*, getObserverTarget, 294, (), (this))
    VIRTUAL_METHOD(WeaponType, getWeaponType, 454, (), (this))
    VIRTUAL_METHOD(WeaponInfo*, getWeaponData, 460, (), (this))
    VIRTUAL_METHOD(float, getInaccuracy, 482, (), (this))

    constexpr auto isPistol() noexcept
    {
        return getWeaponType() == WeaponType::Pistol;
    }

    constexpr auto isSniperRifle() noexcept
    {
        return getWeaponType() == WeaponType::SniperRifle;
    }

    constexpr auto requiresRecoilControl() noexcept
    {
        const auto weaponData = getWeaponData();
        if (weaponData)
            return weaponData->recoilMagnitude < 35.0f && weaponData->recoveryTimeStand > weaponData->cycletime;
        return false;
    }

    bool setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) noexcept
    {
        if (config->misc.fixBoneMatrix) {
            int* render = reinterpret_cast<int*>(this + 0x274);
            int backup = *render;
            Vector absOrigin = getAbsOrigin();
            *render = 0;
            memory->setAbsOrigin(this, origin());
            auto result = VirtualMethod::call<bool, 13>(this + 4, out, maxBones, boneMask, currentTime);
            memory->setAbsOrigin(this, absOrigin);
            *render = backup;
            return result;
        }
        return VirtualMethod::call<bool, 13>(this + 4, out, maxBones, boneMask, currentTime);
    }

    Vector getBonePosition(int bone) noexcept
    {
        if (matrix3x4 boneMatrices[256]; setupBones(boneMatrices, 256, 256, 0.0f))
            return Vector{ boneMatrices[bone][0][3], boneMatrices[bone][1][3], boneMatrices[bone][2][3] };
        else
            return Vector{ };
    }

    auto getEyePosition() noexcept
    {
        Vector vec;
        VirtualMethod::call<void, 284>(this, std::ref(vec));
        return vec;
    }

    bool isVisible(const Vector& position = { }) noexcept
    {
        if (!localPlayer)
            return false;

        Trace trace;
        interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), position ? position : getBonePosition(8) }, 0x46004009, { localPlayer.get() }, trace);
        return trace.entity == this || trace.fraction > 0.97f;
    }
    
    bool isOtherEnemy(Entity* other) noexcept;

    VarMap* getVarMap() noexcept
    {
        return reinterpret_cast<VarMap*>(this + 0x24);
    }
   
    AnimState* getAnimstate() noexcept
    {
        return *reinterpret_cast<AnimState**>(this + 0x3914);
    }

    float getMaxDesyncAngle() noexcept
    {
        const auto animState = getAnimstate();

        if (!animState)
            return 0.0f;

        float yawModifier = (animState->stopToFullRunningFraction * -0.3f - 0.2f) * std::clamp(animState->footSpeed, 0.0f, 1.0f) + 1.0f;

        if (animState->duckAmount > 0.0f)
            yawModifier += (animState->duckAmount * std::clamp(animState->footSpeed2, 0.0f, 1.0f) * (0.5f - yawModifier));

        return animState->velocitySubtractY * yawModifier;
    }

    bool isInReload() noexcept
    {
        return *reinterpret_cast<bool*>(uintptr_t(&clip()) + 0x41);
    }

    auto getAimPunch() noexcept
    {
        Vector vec;
        VirtualMethod::call<void, 345>(this, std::ref(vec));
        return vec;
    }

    auto getUserId() noexcept
    {
        if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(index(), playerInfo))
            return playerInfo.userId;

        return -1;
    }

    [[nodiscard]] auto getPlayerName(bool normalize) noexcept
    {
        std::string playerName = "unknown";

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(index(), playerInfo))
            return playerName;

        playerName = playerInfo.name;

        if (normalize) {
            if (wchar_t wide[128]; MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, 128, wide, 128)) {
                if (wchar_t wideNormalized[128]; NormalizeString(NormalizationKC, wide, -1, wideNormalized, 128)) {
                    if (char nameNormalized[128]; WideCharToMultiByte(CP_UTF8, 0, wideNormalized, -1, nameNormalized, 128, nullptr, nullptr))
                        playerName = nameNormalized;
                }
            }
        }

        playerName.erase(std::remove(playerName.begin(), playerName.end(), '\n'), playerName.cend());
        return playerName;
    }

    NETVAR(body, "CBaseAnimating", "m_nBody", int)
    NETVAR(hitboxSet, "CBaseAnimating", "m_nHitboxSet", int)

    NETVAR(modelIndex, "CBaseEntity", "m_nModelIndex", unsigned)
    NETVAR(origin, "CBaseEntity", "m_vecOrigin", Vector)
    NETVAR_OFFSET(moveType, "CBaseEntity", "m_nRenderMode", 1, MoveType)
    NETVAR(simulationTime, "CBaseEntity", "m_flSimulationTime", float)
    NETVAR(ownerEntity, "CBaseEntity", "m_hOwnerEntity", int)
    NETVAR(team, "CBaseEntity", "m_iTeamNum", int)

    NETVAR(weapons, "CBaseCombatCharacter", "m_hMyWeapons", int[48])
    PNETVAR(wearables, "CBaseCombatCharacter", "m_hMyWearables", int)

    NETVAR(viewModel, "CBasePlayer", "m_hViewModel[0]", int)
    // NETVAR(health, "CBasePlayer", "m_iHealth", int)
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
    NETVAR(reserveAmmoCount, "CBaseCombatWeapon", "m_iPrimaryReserveAmmoCount", int)
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
