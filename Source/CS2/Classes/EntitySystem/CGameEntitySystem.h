#pragma once

#include <cstdint>
#include <CS2/Classes/CUtlMap.h>
#include "CEntityClass.h"

namespace cs2
{

struct CGameEntitySystem {
    using EntityClasses = CUtlMap<const char*, CEntityClass*, std::uint16_t>;
};

}
