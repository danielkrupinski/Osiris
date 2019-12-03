#pragma once

#include "Utils.h"
#include "Vector.h"
#include "MoveData.h"

class GameMovement {
public:

    constexpr void ProcessMovement(Entity* localPlayer, MoveData* move) noexcept
    {
        return callVirtualMethod<void, Entity*, MoveData*>(this, 1, localPlayer, move);
    }

    constexpr void Reset() noexcept
    {
        return callVirtualMethod<void>(this, 2);
    }

    constexpr void StartTrackPredictionErrors(Entity* localPlayer) noexcept
    {
        return callVirtualMethod<void, Entity*>(this, 3, localPlayer);
    }

    constexpr void FinishTrackPredictionErrors(Entity* localPlayer) noexcept
    {
        return callVirtualMethod<void, Entity*>(this, 4, localPlayer);
    }

    constexpr Vector getPlayerViewOffset(bool ducked) noexcept
    {
        return callVirtualMethod<Vector&, bool>(this, 8, ducked);
    }
};
