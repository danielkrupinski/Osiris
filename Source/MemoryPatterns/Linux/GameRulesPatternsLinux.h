#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <GameClasses/OffsetTypes/GameRulesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameRulesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::C_CSGameRules** gameRules() const noexcept
    {
        return patternFinders.clientPatternFinder("83 3D ? ? ? ? 00 74 ? 48 83 C4"_pat).add(2).abs(5).template as<cs2::C_CSGameRules**>();
    }

    [[nodiscard]] RoundStartTimeOffset roundStartTimeOffset() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 10 43 ? F3 0F 5C C1 66 0F EF C9 F3 0F 2A"_pat).add(4).template readOffset<RoundStartTimeOffset>();
    }

    [[nodiscard]] OffsetToRoundRestartTime offsetToRoundRestartTime() const noexcept
    {
        return patternFinders.clientPatternFinder("44 8B 6F ? 48 8B 80 ? ? ? ? 48"_pat).add(3).template readOffset<OffsetToRoundRestartTime>();
    }
};
