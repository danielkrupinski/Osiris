#pragma once

#include <MemoryPatterns/PlayerControllerPatterns.h>

struct PlayerControllerImpl {
    explicit PlayerControllerImpl(const PlayerControllerPatterns& playerControllerPatterns) noexcept
        : offsetToPlayerPawnHandle{playerControllerPatterns.offsetToPlayerPawnHandle()}
    {
    }

    OffsetToPlayerPawnHandle offsetToPlayerPawnHandle;
};
