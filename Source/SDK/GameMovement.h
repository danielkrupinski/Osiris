#pragma once

#include "VirtualMethod.h"

class MoveData;

namespace csgo::pod { struct Entity; }

class GameMovement : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD_V(void, processMovement, 1, (csgo::pod::Entity* localPlayer, MoveData* moveData), (localPlayer, moveData))
};
