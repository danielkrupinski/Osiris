#pragma once

#include <MemoryPatterns/PatternTypes/PlayerControllerPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerControllerPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToBasePawnHandle, CodePattern{"0F B6 81 ? ? ? ? 84 C0 75 ? 8B ? ? ? ? ?"}.add(13).read()>()
            .template addPattern<OffsetToPlayerColor, CodePattern{"8B 96 ? ? ? ? EB 05"}.add(2).read()>();
    }
};
