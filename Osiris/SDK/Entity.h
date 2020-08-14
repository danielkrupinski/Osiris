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
enum Contents
{
	contents_empty = 0x0,
	contents_solid = 0x1,
	contents_window = 0x2,
	contents_aux = 0x4,
	contents_grate = 0x8,
	contents_slime = 0x10,
	contents_water = 0x20,
	contents_blocklos = 0x40,
	contents_opaque = 0x80,
	contents_testfogvolume = 0x100,
	contents_unused = 0x200,
	contents_blocklight = 0x400,
	contents_team1 = 0x800,
	contents_team2 = 0x1000,
	contents_ignore_nodraw_opaque = 0x2000,
	contents_moveable = 0x4000,
	contents_areaportal = 0x8000,
	contents_playerclip = 0x10000,
	contents_monsterclip = 0x20000,
	contents_current0 = 0x40000,
	contents_current90 = 0x80000,
	contents_current180 = 0x100000,
	contents_current270 = 0x200000,
	contents_current_up = 0x400000,
	contents_current_down = 0x800000,
	contents_origin = 0x1000000,
	contents_monster = 0x2000000,
	contents_debris = 0x4000000,
	contents_detail = 0x8000000,
	contents_translucent = 0x10000000,
	contents_ladder = 0x20000000,
	contents_hitbox = 0x40000000,

	last_visible_contents = contents_opaque,
	all_visible_contents = last_visible_contents | last_visible_contents - 1
};

enum surf
{
	surf_light = 0x1,
	surf_sky2d = 0x2,
	surf_sky = 0x4,
	surf_warp = 0x8,
	surf_trans = 0x10,
	surf_noportal = 0x20,
	surf_trigger = 0x40,
	surf_nodraw = 0x80,
	surf_hint = 0x100,
	surf_skip = 0x200,
	surf_nolight = 0x400,
	surf_bumplight = 0x800,
	surf_noshadows = 0x1000,
	surf_nodecals = 0x2000,
	surf_nopaint = surf_nodecals,
	surf_nochop = 0x4000,
	surf_hitbox = 0x8000
};

enum mask
{
	mask_all = 0xFFFFFFFF,
	mask_solid = (contents_solid | contents_moveable | contents_window | contents_monster | contents_grate),
	mask_playersolid = (contents_solid | contents_moveable | contents_playerclip | contents_window | contents_monster | contents_grate),
	mask_npcsolid = (contents_solid | contents_moveable | contents_monsterclip | contents_window | contents_monster | contents_grate),
	mask_npcfluid = (contents_solid | contents_moveable | contents_monsterclip | contents_window | contents_monster),
	mask_water = (contents_water | contents_moveable | contents_slime),
	mask_opaque = (contents_water | contents_moveable | contents_opaque),
	mask_opaque_npc = (mask_opaque | contents_monster),
	mask_blocklos = (contents_solid | contents_moveable | contents_slime),
	mask_blocklos_npc = (mask_blocklos | contents_monster),
	mask_visible = (mask_opaque | contents_ignore_nodraw_opaque),
	mask_visible_npc = (mask_opaque_npc | contents_ignore_nodraw_opaque),
	mask_shot = (contents_solid | contents_moveable | contents_monster | contents_window | contents_debris | contents_hitbox),
	mask_shot_brushonly = (contents_solid | contents_moveable | contents_window | contents_debris),
	mask_shot_hull = (contents_solid | contents_moveable | contents_monster | contents_window | contents_debris | contents_grate),
	mask_shot_portal = (contents_solid | contents_moveable | contents_window | contents_monster),
	mask_solid_brushonly = (contents_solid | contents_moveable | contents_window | contents_grate),
	mask_playersolid_brushonly = (contents_solid | contents_moveable | contents_window | contents_playerclip | contents_grate),
	mask_npcsolid_brushonly = (contents_solid | contents_moveable | contents_window | contents_monsterclip | contents_grate),
	mask_npcworldstatic = (contents_solid | contents_window | contents_monsterclip | contents_grate),
	mask_npcworldstatic_fluid = (contents_solid | contents_window | contents_monsterclip),
	mask_splitareaportal = (contents_water | contents_slime),
	mask_current = (contents_current0 | contents_current90 | contents_current180 | contents_current270 | contents_current_up | contents_current_down),
	mask_deadsolid = (contents_solid | contents_playerclip | contents_window | contents_grate)
};

struct AnimState;

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

enum PlayerFlags {
	ONGROUND = 1 << 0,
	DUCKING = 1 << 1,
	WATERJUMP = 1 << 2,
	ONTRAIN = 1 << 3,
	INRAIN = 1 << 4,
	FROZEN = 1 << 5,
	ATCONTROLS = 1 << 6,
	CLIENT = 1 << 7,
	FAKECLIENT = 1 << 8,
	INWATER = 1 << 9,
	FLY = 1 << 10,
	SWIM = 1 << 11,
	CONVEYOR = 1 << 12,
	NPC = 1 << 13,
	GODMODE = 1 << 14,
	NOTARGET = 1 << 15,
	AIMTARGET = 1 << 16,
	PARTIALGROUND = 1 << 17,
	STATICPROP = 1 << 18,
	GRAPHED = 1 << 19,
	GRENADE = 1 << 20,
	STEPMOVEMENT = 1 << 21,
	DONTTOUCH = 1 << 22,
	BASEVELOCITY = 1 << 23,
	WORLDBRUSH = 1 << 24,
	OBJECT = 1 << 25,
	KILLME = 1 << 26,
	ONFIRE = 1 << 27,
	DISSOLVING = 1 << 28,
	TRANSRAGDOLL = 1 << 29,
	UNBLOCKABLE_BY_PLAYER = 1 << 30
};

enum HitBoxes
{
	HITBOX_HEAD = 0,
	HITBOX_NECK = 1,
	HITBOX_PELVIS = 2,
	HITBOX_BODY = 3,
	HITBOX_THORAX = 4,
	HITBOX_LOWER_CHEST = 5,
	HITBOX_UPPER_CHEST = 6,
	HITBOX_RIGHT_THIGH = 7,
	HITBOX_LEFT_THIGH = 8,
	HITBOX_RIGHT_CALF = 9,
	HITBOX_LEFT_CALF = 10,
	HITBOX_RIGHT_FOOT = 11,
	HITBOX_LEFT_FOOT = 12,
	HITBOX_RIGHT_HAND = 13,
	HITBOX_LEFT_HAND = 14,
	HITBOX_RIGHT_UPPER_ARM = 15,
	HITBOX_RIGHT_FOREARM = 16,
	HITBOX_LEFT_UPPER_ARM = 17,
	HITBOX_LEFT_FOREARM = 18,
	HITBOX_MAX = 19
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
    VIRTUAL_METHOD(const Vector&, getAbsOrigin, 10, (), (this))
    VIRTUAL_METHOD(void, setModelIndex, 75, (int index), (this, index))
    VIRTUAL_METHOD(int, health, 121, (), (this))
    VIRTUAL_METHOD(bool, isAlive, 155, (), (this))
    VIRTUAL_METHOD(bool, isPlayer, 157, (), (this))
    VIRTUAL_METHOD(bool, isWeapon, 165, (), (this))
    VIRTUAL_METHOD(Entity*, getActiveWeapon, 267, (), (this))
    VIRTUAL_METHOD(int, getWeaponSubType, 281, (), (this))
    VIRTUAL_METHOD(Vector, getEyePosition, 284, (), (this))
    VIRTUAL_METHOD(ObsMode, getObserverMode, 293, (), (this))
    VIRTUAL_METHOD(Entity*, getObserverTarget, 294, (), (this))
    VIRTUAL_METHOD(Vector, getAimPunch, 345, (), (this))
    VIRTUAL_METHOD(WeaponType, getWeaponType, 454, (), (this))
    VIRTUAL_METHOD(float, getSpread, 452, (), (this))
    VIRTUAL_METHOD(WeaponInfo*, getWeaponData, 460, (), (this))
    VIRTUAL_METHOD(float, getInaccuracy, 482, (), (this))
    VIRTUAL_METHOD(void, UpdateAccuracyPenalty, 483, (), (this))

    constexpr auto isPistol() noexcept
    {
        return getWeaponType() == WeaponType::Pistol;
    }

    constexpr auto isSniperRifle() noexcept
    {
        return getWeaponType() == WeaponType::SniperRifle;
    }

    constexpr auto isFullAuto() noexcept
    {
        const auto weaponData = getWeaponData();
        if (weaponData)
            return weaponData->fullAuto;
        return false;
    }

	constexpr auto isKnife() noexcept
	{
		return getWeaponType() == WeaponType::Knife;
	}

	constexpr auto isNade() noexcept
	{
		return getWeaponType() == WeaponType::Grenade;
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
            return boneMatrices[bone].origin();
        else
            return Vector{ };
    }

    bool isVisible(const Vector& position = { }) noexcept
    {
        if (!localPlayer)
            return false;

        Trace trace;
        interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), position.notNull() ? position : getBonePosition(8) }, 0x46004009, { localPlayer.get() }, trace);
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

    auto getUserId() noexcept
    {
        if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(index(), playerInfo))
            return playerInfo.userId;

        return -1;
    }

    void getPlayerName(char(&out)[128]) noexcept;
    [[nodiscard]] std::string getPlayerName() noexcept
    {
        char name[128];
        getPlayerName(name);
        return name;
    }

    bool canSee(Entity* other, const Vector& pos) noexcept;
    bool visibleTo(Entity* other) noexcept;

    NETVAR(body, "CBaseAnimating", "m_nBody", int)
    NETVAR(hitboxSet, "CBaseAnimating", "m_nHitboxSet", int)

    NETVAR(modelIndex, "CBaseEntity", "m_nModelIndex", unsigned)
    NETVAR(origin, "CBaseEntity", "m_vecOrigin", Vector)
    NETVAR_OFFSET(moveType, "CBaseEntity", "m_nRenderMode", 1, MoveType)
    NETVAR(simulationTime, "CBaseEntity", "m_flSimulationTime", float)
    NETVAR(ownerEntity, "CBaseEntity", "m_hOwnerEntity", int)
    NETVAR(team, "CBaseEntity", "m_iTeamNum", int)
    NETVAR(spotted, "CBaseEntity", "m_bSpotted", bool)

    NETVAR(weapons, "CBaseCombatCharacter", "m_hMyWeapons", int[48])
    PNETVAR(wearables, "CBaseCombatCharacter", "m_hMyWearables", int)

    NETVAR(viewModel, "CBasePlayer", "m_hViewModel[0]", int)
    NETVAR(fov, "CBasePlayer", "m_iFOV", int)
    NETVAR(fovStart, "CBasePlayer", "m_iFOVStart", int)
    NETVAR(defaultFov, "CBasePlayer", "m_iDefaultFOV", int)
    NETVAR(flags, "CBasePlayer", "m_fFlags", int)
    NETVAR(tickBase, "CBasePlayer", "m_nTickBase", int)
    NETVAR(aimPunchAngle, "CBasePlayer", "m_aimPunchAngle", Vector)
    NETVAR(viewPunchAngle, "CBasePlayer", "m_viewPunchAngle", Vector)
    NETVAR(velocity, "CBasePlayer", "m_vecVelocity[0]", Vector)
    
    NETVAR(armor, "CCSPlayer", "m_ArmorValue", int)
    NETVAR(eyeAngles, "CCSPlayer", "m_angEyeAngles", Vector)
    NETVAR(isScoped, "CCSPlayer", "m_bIsScoped", bool)
    NETVAR(isDefusing, "CCSPlayer", "m_bIsDefusing", bool)
    NETVAR_OFFSET(flashDuration, "CCSPlayer", "m_flFlashMaxAlpha", -8, float)
    NETVAR(flashMaxAlpha, "CCSPlayer", "m_flFlashMaxAlpha", float)
    NETVAR(gunGameImmunity, "CCSPlayer", "m_bGunGameImmunity", bool)
    NETVAR(account, "CCSPlayer", "m_iAccount", int)
    NETVAR(inBombZone, "CCSPlayer", "m_bInBombZone", bool)
    NETVAR(hasDefuser, "CCSPlayer", "m_bHasDefuser", bool)
    NETVAR(hasHelmet, "CCSPlayer", "m_bHasHelmet", bool)
    NETVAR(lby, "CCSPlayer", "m_flLowerBodyYawTarget", float)
    NETVAR(ragdoll, "CCSPlayer", "m_hRagdoll", int)
    NETVAR(shotsFired, "CCSPlayer", "m_iShotsFired", int)
    NETVAR(waitForNoAttack, "CCSPlayer", "m_bWaitForNoAttack", bool)

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

    NETVAR(thrower, "CBaseGrenade", "m_hThrower", int)

    bool isFlashed() noexcept
    {
        return flashDuration() > 75.0f;
    }

    bool grenadeExploded() noexcept
    {
        return *reinterpret_cast<bool*>(this + 0x29E8);
    }
};
