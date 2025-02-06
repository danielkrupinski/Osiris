#pragma once

#include "CEntityIdentity.h"

namespace cs2
{

struct CConcreteEntityList {
    static constexpr auto kNumberOfChunks{64};
    static constexpr auto kNumberOfIdentitiesPerChunk{512};

    using EntityChunk = CEntityIdentity[kNumberOfIdentitiesPerChunk];

    EntityChunk* chunks[kNumberOfChunks];
};

}
