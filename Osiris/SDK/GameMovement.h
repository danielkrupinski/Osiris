#pragma once

#include "Utils.h"

class Entity;
class MoveData;

class GameMovement {
public:
    constexpr auto processMovement(Entity* localPlayer, MoveData* moveData) noexcept
    {
        callVirtualMethod<void>(this, 1, localPlayer, moveData);
    }
};
