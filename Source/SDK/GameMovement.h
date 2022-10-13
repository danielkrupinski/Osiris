#pragma once

#include "VirtualMethod.h"

class MoveData;

class GameMovement : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2_V(void, processMovement, 1, (std::uintptr_t localPlayer, MoveData* moveData), (localPlayer, moveData))
};
