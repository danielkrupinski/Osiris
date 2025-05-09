#pragma once

#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Classes/CUtlVector.h>

namespace cs2
{

struct CCS_PortraitWorld {
    using EntityHandles = CUtlVector<CEntityHandle>;
};

}
