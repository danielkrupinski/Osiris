#pragma once

#include <cstdint>
#include <Platform/Macros/IsPlatform.h>

#if !IS_WIN64()
#include <CS2/Classes/CUtlMap.h>
#endif

#include <CS2/Classes/EntitySystem/CEntityClass.h>

namespace cs2
{

struct CGameEntitySystem {
#if !IS_WIN64()
    using EntityClasses = CUtlMap<const char*, CEntityClass*, std::uint16_t>;
#endif
};

}
