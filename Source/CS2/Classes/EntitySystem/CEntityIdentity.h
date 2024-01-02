#pragma once

#include <Utils/Pad.h>

#include "CEntityHandle.h"

namespace cs2
{

struct CEntityInstance;

struct CEntityIdentity {
    CEntityInstance* entity;
    PAD(8);
    CEntityHandle handle;
    PAD(100);
};

static_assert(sizeof(CEntityIdentity) == 120);

}
