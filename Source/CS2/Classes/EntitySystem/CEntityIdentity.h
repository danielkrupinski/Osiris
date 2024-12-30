#pragma once

#include <Utils/Pad.h>

#include "CEntityHandle.h"

namespace cs2
{

struct CEntityInstance;
struct CEntityClass;

struct CEntityIdentity {
    CEntityInstance* entity;
    CEntityClass* entityClass;
    CEntityHandle handle;
    PAD(100);
};

static_assert(sizeof(CEntityIdentity) == 120);

}
