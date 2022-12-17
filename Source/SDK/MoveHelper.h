#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct EntityPOD;
struct MoveHelperPOD;

class MoveHelper : public VirtualCallableFromPOD<MoveHelper, MoveHelperPOD> {
public:
    VIRTUAL_METHOD(void, setHost, 1, (EntityPOD* host), (host))
};

}
