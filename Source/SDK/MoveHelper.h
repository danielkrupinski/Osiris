#pragma once

#include "VirtualMethod.h"

namespace csgo::pod { struct MoveHelper; }

class MoveHelper : public VirtualCallableFromPOD<MoveHelper, csgo::pod::MoveHelper> {
public:
    VIRTUAL_METHOD2(void, setHost, 1, (std::uintptr_t host), (host))
};
