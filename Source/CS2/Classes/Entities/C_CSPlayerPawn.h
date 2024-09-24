#pragma once

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <Platform/Macros/PlatformSpecific.h>

#include "C_BaseEntity.h"

namespace cs2
{

struct CCSPlayer_HostageServices;
struct CCSPlayer_WeaponServices;

struct C_CSPlayerPawn : C_BaseEntity {
    using m_bGunGameImmunity = bool;
    using m_pWeaponServices = CCSPlayer_WeaponServices*;
    using m_hController = CEntityHandle;
    using m_bIsDefusing = bool;
    using m_bIsGrabbingHostage = bool;
    using m_pHostageServices = CCSPlayer_HostageServices*;
    using m_flFlashBangTime = float;
};

}
