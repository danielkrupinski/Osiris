#pragma once

#include <MemoryPatterns/PatternTypes/GameRulesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameRulesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<GameRulesPointer, CodePattern{"FF 53 ? 48 83 3D ? ? ? ? 00 74"}.add(6).abs(5)>()
            .template addPattern<RoundStartTimeOffset, CodePattern{"EC ? 8B 5F ? ? DA"}.add(4).read()>()
            .template addPattern<OffsetToRoundRestartTime, CodePattern{"10 57 ? F3 0F 11 95"}.add(2).read()>();
    }
};
