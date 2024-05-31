#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <GameClasses/OffsetTypes/GameRulesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameRulesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::C_CSGameRules** gameRules() const noexcept
    {
        return patternFinders.clientPatternFinder("4C 8D ? ? ? ? ? 49 8B ? 48 85 FF 0F 84 ? ? ? ? 41 80 BC 24"_pat).add(3).abs().template as<cs2::C_CSGameRules**>();
    }

    [[nodiscard]] RoundStartTimeOffset roundStartTimeOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 10 43 ? F3 0F 5C C1 66 0F EF C9 F3 0F 2A"_pat).add(4).template readOffset<RoundStartTimeOffset>();
    }
};
