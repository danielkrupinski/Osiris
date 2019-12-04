#pragma once

#include "Utils.h"
#include "Vector.h"
#include "MoveData.h"

class GameMovement {
public:

	constexpr void ProcessMovement(Entity* localPlayer, MoveData* move)
	{
		return callVirtualMethod<void, Entity*, MoveData*>(this, 1, localPlayer, move);
	}

	constexpr void StartTrackPredictionErrors(Entity* localPlayer)
	{
		return callVirtualMethod<void, Entity*>(this, 3, localPlayer);
	}

	constexpr void FinishTrackPredictionErrors(Entity* localPlayer)
	{
		return callVirtualMethod<void, Entity*>(this, 4, localPlayer);
	}

	constexpr Vector getPlayerViewOffset(bool ducked) noexcept
	{
		return callVirtualMethod<Vector&, bool>(this, 8, ducked);
	}
};