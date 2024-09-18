#pragma once

#include <Platform/Macros/PlatformSpecific.h>
#include "C_BaseEntity.h"

namespace cs2
{

struct CBaseAnimGraph : C_BaseEntity {
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCBaseAnimGraph@@", "14CBaseAnimGraph")};
};

}
