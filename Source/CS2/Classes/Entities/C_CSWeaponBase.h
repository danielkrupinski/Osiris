#pragma once

#include <cstdint>

#include "C_BaseEntity.h"

namespace cs2
{

struct C_CSWeaponBase : C_BaseEntity {
    using m_iClip1 = std::int32_t;
};

}
