#pragma once

#include <MemoryPatterns/PatternTypes/GameRulesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameRulesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<GameRulesPointer, CodePattern{"83 3D ? ? ? ? 00 74 ? 48 83 C4"}.add(2).abs(5)>()
            .template addPattern<RoundStartTimeOffset, CodePattern{"F3 0F 10 43 ? F3 0F 5C C1 66 0F EF C9 F3 0F 2A"}.add(4).read()>()
            .template addPattern<OffsetToRoundRestartTime, CodePattern{"44 8B 6F ? 48 8B 80 ? ? ? ? 48"}.add(3).read()>();
    }
};
