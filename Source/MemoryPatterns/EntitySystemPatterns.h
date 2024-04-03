#pragma once

#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <GameClasses/OffsetTypes/EntitySystemOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct EntitySystemPatterns {
    [[nodiscard]] cs2::CGameEntitySystem** entitySystem() const noexcept;
    [[nodiscard]] HighestEntityIndexOffset highestEntityIndexOffset() const noexcept;
    [[nodiscard]] EntityListOffset entityListOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
