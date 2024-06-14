#pragma once

#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>

struct PlayerControllerDeps {
    template <typename PlayerControllerPatterns>
    explicit PlayerControllerDeps(const PlayerControllerPatterns& playerControllerPatterns) noexcept
        : offsetToPlayerPawnHandle{playerControllerPatterns.offsetToPlayerPawnHandle()}
        , offsetToPlayerColor{playerControllerPatterns.offsetToPlayerColor()}
    {
    }

    OffsetToPlayerPawnHandle offsetToPlayerPawnHandle;
    OffsetToPlayerColor offsetToPlayerColor;
};
