#pragma once

#include "CEntityIdentity.h"
#include "CEntityIndex.h"

namespace cs2
{

struct CConcreteEntityList {
    static constexpr auto kNumberOfChunks{64};
    static constexpr auto kNumberOfIdentitiesPerChunk{512};

    using EntityChunk = CEntityIdentity[kNumberOfIdentitiesPerChunk];

    EntityChunk* chunks[kNumberOfChunks];
};

}
