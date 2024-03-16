#pragma once

#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <MemoryPatterns/EntitySystemPatterns.h>

struct EntitySystemImpl {
    explicit EntitySystemImpl(const EntitySystemPatterns& entitySystemPatterns) noexcept
        : thisptr{entitySystemPatterns.entitySystem()}
        , highestEntityIndexOffset{entitySystemPatterns.highestEntityIndexOffset()}
        , entityListOffset{entitySystemPatterns.entityListOffset()}
    {
    }

    cs2::CGameEntitySystem** thisptr;
    HighestEntityIndexOffset highestEntityIndexOffset;
    EntityListOffset entityListOffset;
};
