#pragma once

#include <MemoryPatterns/GameSceneNodePatterns.h>

struct GameSceneNodeImpl {
    explicit GameSceneNodeImpl(const GameSceneNodePatterns& gameSceneNodePatterns) noexcept
        : offsetToAbsOrigin{gameSceneNodePatterns.offsetToAbsOrigin()}
    {
    }

    OffsetToAbsOrigin offsetToAbsOrigin;
};
