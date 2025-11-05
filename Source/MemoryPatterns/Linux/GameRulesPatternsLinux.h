#pragma once

#include <MemoryPatterns/PatternTypes/GameRulesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameRulesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<GameRulesPointer, CodePattern{"FF 53 ? 48 83 3D ? ? ? ? 00 74"}.add(6).abs(5)>()
            .template addPattern<RoundStartTimeOffset, CodePattern{"EC ? 8B 5F ? ? DA"}.add(4).read()>()
            .template addPattern<OffsetToRoundRestartTime, CodePattern{"8D 15 ? ? ? ? F3 0F 10 57 ?"}.add(10).read()>()
            .template addPattern<OffsetToRoundLength, CodePattern{"0F 2A 43 ? 48 8B 5D"}.add(3).read()>()
            .template addPattern<OffsetToRoundWinStatus, CodePattern{"B8 ? ? ? ? 00 0F 85 ? ? ? ? BA"}.add(1).read()>();
    }
};
