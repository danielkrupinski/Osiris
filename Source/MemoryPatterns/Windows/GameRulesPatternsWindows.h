#pragma once

#include <GameClasses/OffsetTypes/GameRulesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameRulesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] RoundStartTimeOffset roundStartTimeOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 10 43 ? 0F 5B C9"_pat).add(4).template readOffset<RoundStartTimeOffset>();
    }
};
