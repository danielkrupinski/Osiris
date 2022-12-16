#pragma once

#include "VirtualMethod.h"

namespace csgo
{

class MoveData;
struct UserCmd;

namespace pod { struct Entity; }
namespace pod { struct MoveHelper; }
namespace pod { struct Prediction; }

class Prediction : public VirtualCallableFromPOD<Prediction, pod::Prediction> {
public:
    VIRTUAL_METHOD_V(void, setupMove, 20, (pod::Entity* localPlayer, UserCmd* cmd, pod::MoveHelper* moveHelper, MoveData* moveData), (localPlayer, cmd, moveHelper, moveData))
    VIRTUAL_METHOD_V(void, finishMove, 21, (pod::Entity* localPlayer, UserCmd* cmd, MoveData* moveData), (localPlayer, cmd, moveData))
};

}
