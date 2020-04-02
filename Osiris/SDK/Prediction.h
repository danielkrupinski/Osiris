#pragma once

#include "VirtualMethod.h"

class Entity;
class MoveData;
class MoveHelper;
struct UserCmd;

class Prediction {
public:
    VIRTUAL_METHOD(void, setupMove, 20, (Entity* localPlayer, UserCmd* cmd, MoveHelper* moveHelper, MoveData* moveData), (this, localPlayer, cmd, moveHelper, moveData))
    VIRTUAL_METHOD(void, finishMove, 21, (Entity* localPlayer, UserCmd* cmd, MoveData* moveData), (this, localPlayer, cmd, moveData))
};
