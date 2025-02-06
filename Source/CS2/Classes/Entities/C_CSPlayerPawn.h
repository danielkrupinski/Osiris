#pragma once

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Classes/SceneObjectUpdaterHandle_t.h>

#include "C_BaseModelEntity.h"

namespace cs2
{

struct CCSPlayer_HostageServices;
struct CCSPlayer_WeaponServices;

struct C_CSPlayerPawn : C_BaseModelEntity {
    using m_bGunGameImmunity = bool;
    using m_pWeaponServices = CCSPlayer_WeaponServices*;
    using m_hController = CEntityHandle;
    using m_bIsDefusing = bool;
    using m_bIsGrabbingHostage = bool;
    using m_pHostageServices = CCSPlayer_HostageServices*;
    using m_flFlashBangTime = float;
    using sceneObjectUpdaterHandle = SceneObjectUpdaterHandle_t*;
};

}
