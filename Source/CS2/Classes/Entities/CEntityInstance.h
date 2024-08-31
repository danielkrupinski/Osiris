#pragma once

#include <Utils/Pad.h>

namespace cs2
{

struct CEntityIdentity;

struct CEntityInstance {
    const void* vmt;
    PAD(8);
    CEntityIdentity* identity;
};

}
