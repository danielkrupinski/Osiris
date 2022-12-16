#pragma once

#include "VirtualMethod.h"

namespace csgo
{

namespace pod { struct Entity; }
namespace pod { struct MoveHelper; }

class MoveHelper : public VirtualCallableFromPOD<MoveHelper, pod::MoveHelper> {
public:
    VIRTUAL_METHOD(void, setHost, 1, (pod::Entity* host), (host))
};

}
