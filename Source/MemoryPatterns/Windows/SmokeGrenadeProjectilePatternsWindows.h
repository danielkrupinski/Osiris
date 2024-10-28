#pragma once

#include <MemoryPatterns/PatternTypes/SmokeGrenadeProjectilePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SmokeGrenadeProjectilePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToDidSmokeEffect, CodePattern{"85 D2 75 ? 38 91 ? ? ? ?"}.add(6).read()>();
    }
};
