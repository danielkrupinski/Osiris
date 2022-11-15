#pragma once

#include "VirtualMethod.h"

class MoveData;

namespace csgo::pod { struct Entity; }
namespace csgo::pod { struct GameMovement; }

class GameMovement : public VirtualCallableFromPOD<GameMovement, csgo::pod::GameMovement> {
public:
    VIRTUAL_METHOD_V(void, processMovement, 1, (csgo::pod::Entity* localPlayer, MoveData* moveData), (localPlayer, moveData))
};
