#pragma once

#include <MemoryPatterns/EntityPatterns.h>

struct EntityImpl {
    explicit EntityImpl(const EntityPatterns& entityPatterns) noexcept
        : offsetToGameSceneNode{entityPatterns.offsetToGameSceneNode()}
        , offsetToHealth{entityPatterns.offsetToHealth()}
        , offsetToLifeState{entityPatterns.offsetToLifeState()}
        , offsetToTeamNumber{entityPatterns.offsetToTeamNumber()}
        , offsetToVData{entityPatterns.offsetToVData()}
    {
    }

    OffsetToGameSceneNode offsetToGameSceneNode;
    OffsetToHealth offsetToHealth;
    OffsetToLifeState offsetToLifeState;
    OffsetToTeamNumber offsetToTeamNumber;
    OffsetToVData offsetToVData;
};
