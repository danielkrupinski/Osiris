#pragma once

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Classes/SceneObjectUpdaterHandle_t.h>

#include <CS2/Classes/Entities/C_BaseModelEntity.h>
#include <CS2/Classes/Vector.h>

namespace cs2
{

struct CCSPlayer_HostageServices;
struct CCSPlayer_WeaponServices;
struct CPlayer_MovementServices { };

struct CCSPlayer_MovementServices : CPlayer_MovementServices {
    using GameTime_t = float;
    using m_fStashGrenadeParameterWhen = GameTime_t;
    using m_bGrenadeParametersStashed = bool;
    using m_angStashedShootAngles = cs2::Vector;
    using m_vecStashedGrenadeThrowPosition = cs2::Vector;
    using m_vecStashedVelocity = cs2::Vector;
};

struct C_BasePlayerPawn : C_BaseModelEntity {
    using m_pMovementServices = CPlayer_MovementServices*;
};

struct C_CSPlayerPawn : C_BasePlayerPawn {
    using m_bGunGameImmunity = bool;
    using m_pWeaponServices = CCSPlayer_WeaponServices*;
    using m_hController = CEntityHandle;
    using m_bIsDefusing = bool;
    using m_bIsGrabbingHostage = bool;
    using m_pHostageServices = CCSPlayer_HostageServices*;
    using m_flFlashBangTime = float;
    using sceneObjectUpdaterHandle = SceneObjectUpdaterHandle_t*;
    using m_bIsScoped = bool;
    using m_angEyeAngles = cs2::Vector;
};

}
