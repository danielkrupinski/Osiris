#pragma once

#include "Entity.h"
#include "UserCmd.h"
#include "MoveData.h"
#include "MoveHelper.h"

class Prediction {
public:
	constexpr void SetupMove(Entity* localPlayer, UserCmd* cmd, MoveHelper* helper, MoveData* move) {
		return callVirtualMethod<void, Entity*, UserCmd*, MoveHelper*, MoveData*>(this, 20, localPlayer, cmd, helper, move);
	}

	constexpr void FinishMove(Entity* localPlayer, UserCmd* cmd, MoveData* move) {
		return callVirtualMethod<void, Entity*, UserCmd*, MoveData*>(this, 21, localPlayer, cmd, move);
	}
};