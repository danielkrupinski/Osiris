#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <GameClasses/OffsetTypes/GameRulesOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using RoundStartTimeOffset = GameRulesOffset<cs2::C_CSGameRules::m_fRoundStartTime, std::int8_t>;

struct GameRulesPatterns {
    [[nodiscard]] RoundStartTimeOffset roundStartTimeOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
