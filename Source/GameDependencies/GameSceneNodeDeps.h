#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>

struct GameSceneNodeDeps {
    GameSceneNodeDeps() = default;

    template <typename GameSceneNodePatterns>
    explicit GameSceneNodeDeps(const GameSceneNodePatterns& gameSceneNodePatterns) noexcept
        : offsetToAbsOrigin{gameSceneNodePatterns.offsetToAbsOrigin()}
        , offsetToOwner{gameSceneNodePatterns.offsetToOwner()}
        , offsetToChild{gameSceneNodePatterns.offsetToChild()}
        , offsetToNextSibling{gameSceneNodePatterns.offsetToNextSibling()}
    {
    }

    OffsetToAbsOrigin offsetToAbsOrigin;
    OffsetToGameSceneNodeOwner offsetToOwner;
    OffsetToChildGameSceneNode offsetToChild;
    OffsetToNextSiblingGameSceneNode offsetToNextSibling;
};
