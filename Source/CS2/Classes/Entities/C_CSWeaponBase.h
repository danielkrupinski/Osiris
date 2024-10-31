#pragma once

#include <cstdint>

#include "C_BaseModelEntity.h"

namespace cs2
{

struct C_CSWeaponBase : C_BaseModelEntity {
    using m_iClip1 = std::int32_t;
};

}
