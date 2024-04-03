#pragma once

#include <GameClasses/OffsetTypes/PlayerPawnOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PlayerPawnPatterns {
    [[nodiscard]] OffsetToPlayerPawnImmunity offsetToPlayerPawnImmunity() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
