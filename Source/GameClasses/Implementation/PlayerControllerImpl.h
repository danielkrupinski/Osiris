#pragma once

#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>

struct PlayerControllerImpl {
    template <typename PlayerControllerPatterns>
    explicit PlayerControllerImpl(const PlayerControllerPatterns& playerControllerPatterns) noexcept
        : offsetToPlayerPawnHandle{playerControllerPatterns.offsetToPlayerPawnHandle()}
        , offsetToPlayerColor{playerControllerPatterns.offsetToPlayerColor()}
    {
    }

    OffsetToPlayerPawnHandle offsetToPlayerPawnHandle;
    OffsetToPlayerColor offsetToPlayerColor;
};
