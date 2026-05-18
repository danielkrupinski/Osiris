#pragma once

#include <MemoryPatterns/PatternTypes/PlayerControllerPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerControllerPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToBasePawnHandle, CodePattern{"84 C0 75 ? 8B 8F ? ? ? ?"}.add(6).read()>()
            .template addPattern<OffsetToPlayerColor, CodePattern{"E8 ? ? ? ? 84 C0 74 ? 8B 83 ? ? ? ? 48 8B"}.add(11).read()>();
    }
};
