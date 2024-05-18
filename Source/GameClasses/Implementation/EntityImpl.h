#pragma once

#include <GameClasses/OffsetTypes/EntityOffset.h>

struct EntityImpl {
    template <typename EntityPatterns>
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
