#pragma once

#include "VirtualMethod.h"

namespace csgo
{

struct EntityPOD;
struct GameMovementPOD;
struct MoveData;

struct GameMovement : GameClass<GameMovement, GameMovementPOD> {
    VIRTUAL_METHOD_V(void, processMovement, 1, (EntityPOD* localPlayer, MoveData* moveData), (localPlayer, moveData))
};

}
