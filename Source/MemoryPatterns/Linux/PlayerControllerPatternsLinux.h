#pragma once

#include <MemoryPatterns/PatternTypes/PlayerControllerPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerControllerPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPawnHandle, CodePattern{"84 C0 75 ? 8B 8F ? ? ? ?"}.add(6).read()>()
            .template addPattern<OffsetToPlayerColor, CodePattern{"49 63 9C 24 ? ? ? ? 83 FB 04"}.add(4).read()>();
    }
};
