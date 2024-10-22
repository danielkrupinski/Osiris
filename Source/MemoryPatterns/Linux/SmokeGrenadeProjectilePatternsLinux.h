#pragma once

#include <MemoryPatterns/PatternTypes/SmokeGrenadeProjectilePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SmokeGrenadeProjectilePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToDidSmokeEffect, CodePattern{"85 F6 75 ? 80 BF ? ? ? ?"}.add(6).read()>();
    }
};
