#pragma once

#include <functional>
#include <string>

#include "AnimState.h"
#include "Vector.h"
#include "VirtualMethod.h"
#include "WeaponData.h"
#include "WeaponId.h"

#include "../Netvars.h"

#include <Interfaces/EngineInterfaces.h>
#include <Platform/Macros/IsPlatform.h>
#include <Platform/Macros/PlatformSpecific.h>

namespace csgo { struct EconItemViewPOD; }

class Memory;
class OtherInterfaces;

namespace csgo { struct AnimState; }
namespace csgo { struct ClientClass; }

enum class MoveType {
    NOCLIP = 8,
    LADDER = 9
};

enum class ObsMode {
    None = 0,
    Deathcam,
    Freezecam,
    Fixed,
    InEye,
    Chase,
    Roaming
};

namespace csgo
{

enum class Team {
    None = 0,
    Spectators,
    TT,
    CT
};

}

namespace csgo
{

struct Model;
struct VarMap;

struct matrix3x4;
struct CollideablePOD;
struct NetworkablePOD;
struct RenderablePOD;

struct Collideable : GameClass<Collideable, CollideablePOD> {
    VIRTUAL_METHOD(const Vector&, obbMins, 1, (), ())
    VIRTUAL_METHOD(const Vector&, obbMaxs, 2, (), ())
};

struct Networkable : GameClass<Networkable, NetworkablePOD> {
    VIRTUAL_METHOD(void, release, 1, (), ())
    VIRTUAL_METHOD(ClientClass*, getClientClass, 2, (), ())
    VIRTUAL_METHOD(void, onDataChanged, 5, (int updateType), (updateType))
    VIRTUAL_METHOD(void, preDataUpdate, 6, (int updateType), (updateType))
    VIRTUAL_METHOD(void, postDataUpdate, 7, (int updateType), (updateType))
    VIRTUAL_METHOD(bool, isDormant, 9, (), ())
    VIRTUAL_METHOD(int, index, 10, (), ())
    VIRTUAL_METHOD(void, setDestroyedOnRecreateEntities, 13, (), ())
};

struct Renderable : GameClass<Renderable, RenderablePOD> {
#if IS_WIN32()
    VIRTUAL_METHOD(bool, shouldDraw, 3, (), ())
#endif

    VIRTUAL_METHOD(const Model*, getModel, 8, (), ())
    VIRTUAL_METHOD(const matrix3x4&, toWorldTransform, 32, (), ())
    VIRTUAL_METHOD(bool, setupBones, 13, (matrix3x4* out, int maxBones, int boneMask, float currentTime), (out, maxBones, boneMask, currentTime))
};

struct EntityPOD;

struct Entity : GameClass<Entity, EntityPOD> {
    [[nodiscard]] auto getNetworkable() const noexcept
    {
        return Networkable::from(getInvoker(), reinterpret_cast<NetworkablePOD*>(getThis() + sizeof(std::uintptr_t) * 2));
    }

    [[nodiscard]] auto getRenderable() const noexcept
    {
        return Renderable::from(getInvoker(), reinterpret_cast<RenderablePOD*>(getThis() + sizeof(std::uintptr_t)));
    }

    bool shouldDraw() const
    {
#if !IS_WIN32()
        return call<bool, 149>();
#else
        return getRenderable().shouldDraw();
#endif
    }

    VIRTUAL_METHOD_V(int&, handle, 2, (), ())
    VIRTUAL_METHOD_V(CollideablePOD*, getCollideable, 3, (), ())

    VIRTUAL_METHOD(const Vector&, getAbsOrigin, WIN32_LINUX(10, 12), (), ())
    VIRTUAL_METHOD(void, setModelIndex, WIN32_LINUX(75, 111), (int index), (index))
    VIRTUAL_METHOD(bool, getAttachment, WIN32_LINUX(84, 122), (int index, Vector& origin), (index, &origin))
    VIRTUAL_METHOD(Team, getTeamNumber, WIN32_LINUX(88, 128), (), ())
#if IS_WIN32()
    VIRTUAL_METHOD(bool, initializeAsClientEntity, 97, (const char* modelName, bool renderWithViewmodels), (modelName, renderWithViewmodels))
#endif
    VIRTUAL_METHOD(int, health, WIN32_LINUX(122, 167), (), ())
    VIRTUAL_METHOD(bool, isAlive, WIN32_LINUX(156, 208), (), ())
    VIRTUAL_METHOD(bool, isPlayer, WIN32_LINUX(158, 210), (), ())
    VIRTUAL_METHOD(bool, isWeapon, WIN32_LINUX(166, 218), (), ())
    VIRTUAL_METHOD(EntityPOD*, getActiveWeapon, WIN32_LINUX(268, 331), (), ())
    VIRTUAL_METHOD(int, getWeaponSubType, WIN32_LINUX(282, 350), (), ())
    VIRTUAL_METHOD(ObsMode, getObserverMode, WIN32_LINUX(294, 357), (), ())
    VIRTUAL_METHOD(EntityPOD*, getObserverTarget, WIN32_LINUX(295, 358), (), ())
    VIRTUAL_METHOD(WeaponType, getWeaponType, WIN32_LINUX(455, 523), (), ())
    VIRTUAL_METHOD(WeaponInfo*, getWeaponData, WIN32_LINUX(461, 529), (), ())
    VIRTUAL_METHOD(int, getMuzzleAttachmentIndex1stPerson, WIN32_LINUX(468, 536), (EntityPOD* viewModel), (viewModel))
    VIRTUAL_METHOD(int, getMuzzleAttachmentIndex3rdPerson, WIN32_LINUX(469, 537), (), ())
    VIRTUAL_METHOD(float, getInaccuracy, WIN32_LINUX(483, 551), (), ())

#if IS_WIN32()
    auto getEyePosition() const noexcept
    {
        Vector v;
        call<void, 285>(&v);
        return v;
    }

    auto getAimPunch() const noexcept
    {
        Vector v;
        call<void, 346>(&v);
        return v;
    }
#else
    VIRTUAL_METHOD(Vector, getEyePosition, 348, (), ())
    VIRTUAL_METHOD(Vector, getAimPunch, 409, (), ())
#endif

    auto isPistol() const noexcept { return getWeaponType() == WeaponType::Pistol; }
    auto isSniperRifle() const noexcept { return getWeaponType() == WeaponType::SniperRifle; }
    auto isGrenade() const noexcept { return getWeaponType() == WeaponType::Grenade; }

    auto isFullAuto() const noexcept
    {
        const auto weaponData = getWeaponData();
        if (weaponData)
            return weaponData->fullAuto;
        return false;
    }

    auto requiresRecoilControl() const noexcept
    {
        const auto weaponData = getWeaponData();
        if (weaponData)
            return weaponData->recoilMagnitude < 35.0f && weaponData->recoveryTimeStand > weaponData->cycletime;
        return false;
    }

    bool setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) const noexcept;
    Vector getBonePosition(int bone) const noexcept;

    bool isVisible(const EngineTrace& engineTrace, const Vector& position = { }) const noexcept;
    bool isOtherEnemy(const Memory& memory, const Entity& other) const noexcept;

    VarMap& getVarMap() const noexcept
    {
        return *reinterpret_cast<VarMap*>(getThis() + WIN32_LINUX(0x24, 0x48));
    }

    AnimState* getAnimstate() const noexcept
    {
#if IS_WIN32()
        return *reinterpret_cast<AnimState**>(getThis() + 0x3914);
#else
        return nullptr;
#endif
    }

    float getMaxDesyncAngle() const noexcept;

    bool isInReload() const noexcept
    {
        return *reinterpret_cast<bool*>(uintptr_t(&clip()) + WIN32_LINUX(0x41, 0x45));
    }

    int getUserId(const Engine& engine) const noexcept;
    std::uint64_t getSteamId(const Engine& engine) const noexcept;

    void getPlayerName(const OtherInterfaces& interfaces, const Memory& memory, char(&out)[128]) const noexcept;
    [[nodiscard]] std::string getPlayerName(const OtherInterfaces& interfaces, const Memory& memory) const noexcept
    {
        char name[128];
        getPlayerName(interfaces, memory, name);
        return name;
    }

    bool canSee(const EngineTrace& engineTrace, const Memory& memory, const Entity& other, const Vector& pos) const noexcept;
    bool visibleTo(const EngineInterfaces& engineInterfaces, const Memory& memory, const Entity& other) const noexcept;

    NETVAR2(body, "CBaseAnimating", "m_nBody", int)
    NETVAR2(hitboxSet, "CBaseAnimating", "m_nHitboxSet", int)

    NETVAR2(modelIndex, "CBaseEntity", "m_nModelIndex", unsigned)
    NETVAR2(origin, "CBaseEntity", "m_vecOrigin", Vector)
    NETVAR_OFFSET2(moveType, "CBaseEntity", "m_nRenderMode", 1, MoveType)
    NETVAR2(simulationTime, "CBaseEntity", "m_flSimulationTime", float)
    NETVAR2(ownerEntity, "CBaseEntity", "m_hOwnerEntity", int)
    NETVAR2(spotted, "CBaseEntity", "m_bSpotted", bool)
    NETVAR2(spottedByMask, "CBaseEntity", "m_bSpottedByMask", std::uint32_t)

    NETVAR2(weapons, "CBaseCombatCharacter", "m_hMyWeapons", int[64])
    PNETVAR2(wearables, "CBaseCombatCharacter", "m_hMyWearables", int)

    NETVAR2(viewModel, "CBasePlayer", "m_hViewModel[0]", int)
    NETVAR2(fov, "CBasePlayer", "m_iFOV", int)
    NETVAR2(fovStart, "CBasePlayer", "m_iFOVStart", int)
    NETVAR2(defaultFov, "CBasePlayer", "m_iDefaultFOV", int)
    NETVAR2(flags, "CBasePlayer", "m_fFlags", int)
    NETVAR2(tickBase, "CBasePlayer", "m_nTickBase", int)
    NETVAR2(aimPunchAngle, "CBasePlayer", "m_aimPunchAngle", Vector)
    NETVAR2(viewPunchAngle, "CBasePlayer", "m_viewPunchAngle", Vector)
    NETVAR2(velocity, "CBasePlayer", "m_vecVelocity[0]", Vector)
    NETVAR2(lastPlaceName, "CBasePlayer", "m_szLastPlaceName", char[18])

    NETVAR2(armor, "CCSPlayer", "m_ArmorValue", int)
    NETVAR2(eyeAngles, "CCSPlayer", "m_angEyeAngles", Vector)
    NETVAR2(isScoped, "CCSPlayer", "m_bIsScoped", bool)
    NETVAR2(isDefusing, "CCSPlayer", "m_bIsDefusing", bool)
    NETVAR_OFFSET2(flashDuration, "CCSPlayer", "m_flFlashMaxAlpha", -8, float)
    NETVAR2(flashMaxAlpha, "CCSPlayer", "m_flFlashMaxAlpha", float)
    NETVAR2(gunGameImmunity, "CCSPlayer", "m_bGunGameImmunity", bool)
    NETVAR2(account, "CCSPlayer", "m_iAccount", int)
    NETVAR2(inBombZone, "CCSPlayer", "m_bInBombZone", bool)
    NETVAR2(hasDefuser, "CCSPlayer", "m_bHasDefuser", bool)
    NETVAR2(hasHelmet, "CCSPlayer", "m_bHasHelmet", bool)
    NETVAR2(lby, "CCSPlayer", "m_flLowerBodyYawTarget", float)
    NETVAR2(ragdoll, "CCSPlayer", "m_hRagdoll", int)
    NETVAR2(shotsFired, "CCSPlayer", "m_iShotsFired", int)
    NETVAR2(waitForNoAttack, "CCSPlayer", "m_bWaitForNoAttack", bool)
    NETVAR2(playerPatchIndices, "CCSPlayer", "m_vecPlayerPatchEconIndices", int[5])

    NETVAR2(viewModelIndex, "CBaseCombatWeapon", "m_iViewModelIndex", int)
    NETVAR2(worldModelIndex, "CBaseCombatWeapon", "m_iWorldModelIndex", int)
    NETVAR2(worldDroppedModelIndex, "CBaseCombatWeapon", "m_iWorldDroppedModelIndex", int)
    NETVAR2(weaponWorldModel, "CBaseCombatWeapon", "m_hWeaponWorldModel", int)
    NETVAR2(clip, "CBaseCombatWeapon", "m_iClip1", int)
    NETVAR2(reserveAmmoCount, "CBaseCombatWeapon", "m_iPrimaryReserveAmmoCount", int)
    NETVAR2(nextPrimaryAttack, "CBaseCombatWeapon", "m_flNextPrimaryAttack", float)

    NETVAR2(nextAttack, "CBaseCombatCharacter", "m_flNextAttack", float)

    NETVAR2(accountID, "CBaseAttributableItem", "m_iAccountID", int)
    NETVAR2(itemDefinitionIndex, "CBaseAttributableItem", "m_iItemDefinitionIndex", WeaponId)
    NETVAR2(itemIDHigh, "CBaseAttributableItem", "m_iItemIDHigh", std::uint32_t)
    NETVAR2(itemIDLow, "CBaseAttributableItem", "m_iItemIDLow", std::uint32_t)
    NETVAR2(entityQuality, "CBaseAttributableItem", "m_iEntityQuality", int)
    NETVAR2(initialized, "CBaseAttributableItem", "m_bInitialized", bool)
    NETVAR2(econItemView, "CBaseAttributableItem", "m_Item", EconItemViewPOD)
    NETVAR2(originalOwnerXuidLow, "CBaseAttributableItem", "m_OriginalOwnerXuidLow", std::uint32_t)
    NETVAR2(originalOwnerXuidHigh, "CBaseAttributableItem", "m_OriginalOwnerXuidHigh", std::uint32_t)

    NETVAR2(owner, "CBaseViewModel", "m_hOwner", int)
    NETVAR2(weapon, "CBaseViewModel", "m_hWeapon", int)

    NETVAR2(c4StartedArming, "CC4", "m_bStartedArming", bool)

    NETVAR2(tabletReceptionIsBlocked, "CTablet", "m_bTabletReceptionIsBlocked", bool)

    NETVAR2(droneTarget, "CDrone", "m_hMoveToThisEntity", int)

    NETVAR2(thrower, "CBaseGrenade", "m_hThrower", int)

    NETVAR2(mapHasBombTarget, "CCSGameRulesProxy", "m_bMapHasBombTarget", bool)

    NETVAR2(fireXDelta, "CInferno", "m_fireXDelta", int[100])
    NETVAR2(fireYDelta, "CInferno", "m_fireYDelta", int[100])
    NETVAR2(fireZDelta, "CInferno", "m_fireZDelta", int[100])
    NETVAR2(fireIsBurning, "CInferno", "m_bFireIsBurning", bool[100])
    NETVAR2(fireCount, "CInferno", "m_fireCount", int)

    bool isFlashed() const noexcept
    {
        return flashDuration() > 75.0f;
    }

    std::uint64_t originalOwnerXuid() const noexcept
    {
        return (std::uint64_t(originalOwnerXuidHigh()) << 32) | originalOwnerXuidLow();
    }

    std::uint64_t itemID() const noexcept
    {
        return (std::uint64_t(itemIDHigh()) << 32) | itemIDLow();
    }

    [[nodiscard]] bool isOnGround() const noexcept
    {
        return (flags() & 1) != 0;
    }
};

}
