#pragma once

#include "EntitySystem/CEntityHandle.h"
#include "CUtlVector.h"

namespace cs2
{

struct CCSPlayer_WeaponServices {
    using m_hActiveWeapon = CEntityHandle;
    using m_hMyWeapons = CUtlVector<CEntityHandle>;
};

}
