#pragma once

#include "CEntitySubclassVDataBase.h"
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <Utils/ToUnderlying.h>

namespace cs2
{

struct CCSWeaponBaseVData : CEntitySubclassVDataBase {
    using m_szName = const char*;
    using m_flSpread = float[toUnderlying(cs2::CSWeaponMode::Max)];
};

}
