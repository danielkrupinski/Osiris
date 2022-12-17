#pragma once

#include "VirtualMethod.h"

namespace csgo
{

class MoveData;

struct EntityPOD;
struct GameMovementPOD;

class GameMovement : public VirtualCallableFromPOD<GameMovement, GameMovementPOD> {
public:
    VIRTUAL_METHOD_V(void, processMovement, 1, (EntityPOD* localPlayer, MoveData* moveData), (localPlayer, moveData))
};

}
