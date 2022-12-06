#pragma once

#include "VirtualMethod.h"

namespace csgo::pod { struct Entity; }
namespace csgo::pod { struct MoveHelper; }

class MoveHelper : public VirtualCallableFromPOD<MoveHelper, csgo::pod::MoveHelper> {
public:
    VIRTUAL_METHOD(void, setHost, 1, (csgo::pod::Entity* host), (host))
};
