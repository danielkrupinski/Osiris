#pragma once

#include "CEntitySubclassVDataBase.h"

namespace cs2
{

struct CCSWeaponBaseVData : CEntitySubclassVDataBase {
    using m_szName = const char*;
    using m_flSpread = float[2];
};

}
