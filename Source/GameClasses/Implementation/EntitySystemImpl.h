#pragma once

#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <MemoryPatterns/EntitySystemPatterns.h>

struct EntitySystemImpl {
    explicit EntitySystemImpl(const EntitySystemPatterns& entitySystemPatterns) noexcept
        : entitySystem{entitySystemPatterns.entitySystem()}
        , highestEntityIndexOffset{entitySystemPatterns.highestEntityIndexOffset()}
        , entityListOffset{entitySystemPatterns.entityListOffset()}
    {
    }

    cs2::CGameEntitySystem** entitySystem;
    HighestEntityIndexOffset highestEntityIndexOffset;
    EntityListOffset entityListOffset;
};
