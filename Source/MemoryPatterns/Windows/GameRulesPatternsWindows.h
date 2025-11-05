#pragma once

#include <MemoryPatterns/PatternTypes/GameRulesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameRulesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<GameRulesPointer, CodePattern{"F6 ? ? 0F 85 ? ? ? ? ? 8B ? ? ? ? ? ? 85 ? 0F"}.add(12).abs()>()
            .template addPattern<RoundStartTimeOffset, CodePattern{"41 ? 48 8B C2 F3 0F 11"}.add(1).read()>()
            .template addPattern<OffsetToRoundRestartTime, CodePattern{"0F 84 ? ? ? ? F3 0F 10 43 ? 48 8D 55 ? F3"}.add(10).read()>()
            .template addPattern<OffsetToRoundLength, CodePattern{"43 ? 0F 5B C0 48"}.add(1).read()>()
            .template addPattern<OffsetToRoundWinStatus, CodePattern{"8D 35 ? ? ? ? 8B ? ? ? ? ? EB"}.add(8).read()>();
    }
};
