#pragma once

#include "VirtualMethod.h"

class Entity;
class MoveData;
class MoveHelper;
struct UserCmd;

namespace csgo::pod { struct Entity; }
namespace csgo::pod { struct MoveHelper; }
namespace csgo::pod { struct Prediction; }

class Prediction : public VirtualCallableFromPOD<Prediction, csgo::pod::Prediction> {
public:
    VIRTUAL_METHOD_V(void, setupMove, 20, (csgo::pod::Entity* localPlayer, UserCmd* cmd, csgo::pod::MoveHelper* moveHelper, MoveData* moveData), (localPlayer, cmd, moveHelper, moveData))
    VIRTUAL_METHOD_V(void, finishMove, 21, (csgo::pod::Entity* localPlayer, UserCmd* cmd, MoveData* moveData), (localPlayer, cmd, moveData))
};
