#pragma once

#include <MemoryPatterns/PatternTypes/PlayerControllerPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerControllerPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPawnHandle, CodePattern{"8B 97 ? ? ? ? 48 89 FB 83 FA FF"}.add(2).read()>()
            .template addPattern<OffsetToPlayerColor, CodePattern{"49 63 9C 24 ? ? ? ? 83 FB 04"}.add(4).read()>();
    }
};
