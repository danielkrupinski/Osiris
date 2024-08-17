#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <GameClasses/OffsetTypes/GameRulesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameRulesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::C_CSGameRules** gameRules() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 0D ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 80 BE"_pat).add(3).abs().template as<cs2::C_CSGameRules**>();
    }

    [[nodiscard]] RoundStartTimeOffset roundStartTimeOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 10 43 ? 0F 5B C9"_pat).add(4).template readOffset<RoundStartTimeOffset>();
    }

    [[nodiscard]] OffsetToRoundRestartTime offsetToRoundRestartTime() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 10 71 ? 48 8B 01"_pat).add(4).template readOffset<OffsetToRoundRestartTime>();
    }
};
