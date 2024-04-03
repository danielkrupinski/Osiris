#pragma once

#include <GameClasses/OffsetTypes/GameRulesOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct GameRulesPatterns {
    [[nodiscard]] RoundStartTimeOffset roundStartTimeOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
