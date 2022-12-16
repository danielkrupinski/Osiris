#pragma once

#include "VirtualMethod.h"

namespace csgo
{

class MoveData;

namespace pod { struct Entity; }
namespace pod { struct GameMovement; }

class GameMovement : public VirtualCallableFromPOD<GameMovement, pod::GameMovement> {
public:
    VIRTUAL_METHOD_V(void, processMovement, 1, (pod::Entity* localPlayer, MoveData* moveData), (localPlayer, moveData))
};

}
