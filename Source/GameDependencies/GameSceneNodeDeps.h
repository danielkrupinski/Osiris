#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>

struct GameSceneNodeDeps {
    template <typename GameSceneNodePatterns>
    explicit GameSceneNodeDeps(const GameSceneNodePatterns& gameSceneNodePatterns) noexcept
        : offsetToAbsOrigin{gameSceneNodePatterns.offsetToAbsOrigin()}
    {
    }

    OffsetToAbsOrigin offsetToAbsOrigin;
};
