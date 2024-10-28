#pragma once

#include <MemoryPatterns/PatternTypes/PlayerControllerPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerControllerPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPawnHandle, CodePattern{"C1 48 85 C9 74 ? 8B ? ? ? ? ? 83"}.add(8).read()>()
            .template addPattern<OffsetToPlayerColor, CodePattern{"E8 ? ? ? ? 84 C0 74 ? 41 8B ? ? ? ? ? EB"}.add(12).read()>();
    }
};
