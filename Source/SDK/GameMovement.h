#pragma once

#include "VirtualMethod.h"

class Entity;
class MoveData;

class GameMovement : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2_V(void, processMovement, 1, (Entity* localPlayer, MoveData* moveData), (localPlayer, moveData))
};
