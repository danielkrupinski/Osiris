#pragma once

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <Platform/Macros/PlatformSpecific.h>

#include "C_BaseEntity.h"

namespace cs2
{

struct CCSPlayer_WeaponServices;

struct C_CSPlayerPawn : C_BaseEntity {
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVC_CSPlayerPawn@@", "14C_CSPlayerPawn")};

    using m_bGunGameImmunity = bool;
    using m_pWeaponServices = CCSPlayer_WeaponServices*;
    using m_hController = CEntityHandle;
    using m_bIsDefusing = bool;
};

}
