#pragma once

#include "Utils.h"

class Entity;
class MoveData;
class MoveHelper;
struct UserCmd;

class Prediction {
public:
    constexpr void setupMove(Entity* localPlayer, UserCmd* cmd, MoveHelper* moveHelper, MoveData* moveData) noexcept
    {
        callVirtualMethod<void>(this, 20, localPlayer, cmd, moveHelper, moveData);
    }

    constexpr void finishMove(Entity* localPlayer, UserCmd* cmd, MoveData* moveData) noexcept
    {
        callVirtualMethod<void>(this, 21, localPlayer, cmd, moveData);
    }
};
