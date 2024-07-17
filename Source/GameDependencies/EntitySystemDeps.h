#pragma once

#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <GameClasses/OffsetTypes/EntitySystemOffset.h>

struct EntitySystemDeps {
    template <typename EntitySystemPatterns>
    explicit EntitySystemDeps(const EntitySystemPatterns& entitySystemPatterns) noexcept
        : entitySystem{entitySystemPatterns.entitySystem()}
        , highestEntityIndexOffset{entitySystemPatterns.highestEntityIndexOffset()}
        , entityListOffset{entitySystemPatterns.entityListOffset()}
    {
    }

    cs2::CGameEntitySystem** entitySystem;
    HighestEntityIndexOffset highestEntityIndexOffset;
    EntityListOffset entityListOffset;
};
