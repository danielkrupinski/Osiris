#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class Entity;
class MoveData;

class GameMovement {
public:
    INCONSTRUCTIBLE(GameMovement)

    VIRTUAL_METHOD_V(void, processMovement, 1, (Entity* localPlayer, MoveData* moveData), (this, localPlayer, moveData))
};
