#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>

struct GameSceneNodeImpl {
    template <typename GameSceneNodePatterns>
    explicit GameSceneNodeImpl(const GameSceneNodePatterns& gameSceneNodePatterns) noexcept
        : offsetToAbsOrigin{gameSceneNodePatterns.offsetToAbsOrigin()}
    {
    }

    OffsetToAbsOrigin offsetToAbsOrigin;
};
