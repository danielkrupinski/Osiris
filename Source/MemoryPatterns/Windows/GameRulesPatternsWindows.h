#pragma once

#include <MemoryPatterns/PatternTypes/GameRulesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameRulesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<GameRulesPointer, CodePattern{"48 8B 0D ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 80 BE"}.add(3).abs()>()
            .template addPattern<RoundStartTimeOffset, CodePattern{"F3 0F 10 43 ? 0F 5B C9"}.add(4).read()>()
            .template addPattern<OffsetToRoundRestartTime, CodePattern{"F3 0F 10 71 ? 48 8B 01"}.add(4).read() > ();
    }
};
