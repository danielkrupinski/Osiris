#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

class Entity;
class MoveData;
class MoveHelper;
struct UserCmd;

class Prediction {
public:
    INCONSTRUCTIBLE(Prediction)

    VIRTUAL_METHOD_V(void, setupMove, 20, (Entity* localPlayer, UserCmd* cmd, MoveHelper* moveHelper, MoveData* moveData), (this, localPlayer, cmd, moveHelper, moveData))
    VIRTUAL_METHOD_V(void, finishMove, 21, (Entity* localPlayer, UserCmd* cmd, MoveData* moveData), (this, localPlayer, cmd, moveData))
};
