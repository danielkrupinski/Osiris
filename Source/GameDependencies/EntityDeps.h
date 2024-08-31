#pragma once

#include <GameClasses/OffsetTypes/EntityOffset.h>

struct EntityDeps {
    template <typename EntityPatterns>
    explicit EntityDeps(const EntityPatterns& entityPatterns) noexcept
        : offsetToGameSceneNode{entityPatterns.offsetToGameSceneNode()}
        , offsetToHealth{entityPatterns.offsetToHealth()}
        , offsetToLifeState{entityPatterns.offsetToLifeState()}
        , offsetToTeamNumber{entityPatterns.offsetToTeamNumber()}
        , offsetToVData{entityPatterns.offsetToVData()}
        , offsetToRenderComponent{entityPatterns.offsetToRenderComponent()}
    {
    }

    OffsetToGameSceneNode offsetToGameSceneNode;
    OffsetToHealth offsetToHealth;
    OffsetToLifeState offsetToLifeState;
    OffsetToTeamNumber offsetToTeamNumber;
    OffsetToVData offsetToVData;
    OffsetToRenderComponent offsetToRenderComponent;
};
