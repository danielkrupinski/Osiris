#pragma once

#include <MemoryPatterns/EntityPatterns.h>

struct EntityImpl {
    explicit EntityImpl(const EntityPatterns& entityPatterns) noexcept
        : offsetToGameSceneNode{entityPatterns.offsetToGameSceneNode()}
        , offsetToHealth{entityPatterns.offsetToHealth()}
        , offsetToTeamNumber{entityPatterns.offsetToTeamNumber()}
    {
    }

    OffsetToGameSceneNode offsetToGameSceneNode;
    OffsetToHealth offsetToHealth;
    OffsetToTeamNumber offsetToTeamNumber;
};
