#pragma once

#include "VirtualMethod.h"

class Entity;
class MoveData;

class GameMovement {
public:
    VIRTUAL_METHOD(void, processMovement, 1, (Entity* localPlayer, MoveData* moveData), (this, localPlayer, moveData))
};
