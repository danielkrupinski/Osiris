#pragma once

#include "VirtualMethod.h"

class Entity;
class MoveData;
class MoveHelper;
struct UserCmd;

namespace csgo::pod { struct Entity; }
namespace csgo::pod { struct MoveHelper; }

class Prediction : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD_V(void, setupMove, 20, (csgo::pod::Entity* localPlayer, UserCmd* cmd, csgo::pod::MoveHelper* moveHelper, MoveData* moveData), (localPlayer, cmd, moveHelper, moveData))
    VIRTUAL_METHOD_V(void, finishMove, 21, (csgo::pod::Entity* localPlayer, UserCmd* cmd, MoveData* moveData), (localPlayer, cmd, moveData))
};
