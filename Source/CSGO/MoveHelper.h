#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct EntityPOD;
struct MoveHelperPOD;

struct MoveHelper : VirtualCallableFromPOD<MoveHelper, MoveHelperPOD> {
    VIRTUAL_METHOD(void, setHost, 1, (EntityPOD* host), (host))
};

}
