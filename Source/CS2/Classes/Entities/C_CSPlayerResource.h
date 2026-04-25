#pragma once

#include "C_BaseEntity.h"
#include <CS2/Classes/Vector.h>
#include <Platform/Macros/PlatformSpecific.h>

namespace cs2
{

struct C_CSPlayerResource : C_BaseEntity {
    using GetBombsiteACenter = WIN64_LINUX(void(C_CSPlayerResource* thisptr, Vector* out), Vector(C_CSPlayerResource* thisptr));
    using GetBombsiteBCenter = WIN64_LINUX(void(C_CSPlayerResource* thisptr, Vector* out), Vector(C_CSPlayerResource* thisptr));
};

}
