#pragma once

#include "Entity.h"
#include "UserCmd.h"
#include "MoveData.h"
#include "MoveHelper.h"

class Prediction {
public:
    constexpr void CheckMovingGround(Entity* localPlayer, double frametime) noexcept
    {
        return callVirtualMethod<void, Entity*, double>(this, 18, localPlayer, frametime);
    }

    constexpr void SetupMove(Entity* localPlayer, UserCmd* cmd, MoveHelper* helper, MoveData* move) noexcept
    {
		return callVirtualMethod<void, Entity*, UserCmd*, MoveHelper*, MoveData*>(this, 20, localPlayer, cmd, helper, move);
	}

    constexpr void FinishMove(Entity* localPlayer, UserCmd* cmd, MoveData* move) noexcept
    {
		return callVirtualMethod<void, Entity*, UserCmd*, MoveData*>(this, 21, localPlayer, cmd, move);
	}
};