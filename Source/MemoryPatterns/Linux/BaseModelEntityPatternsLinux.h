#pragma once

#include <MemoryPatterns/PatternTypes/BaseModelEntityPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct BaseModelEntityPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGlowProperty, CodePattern{"90 ? ? ? ? 4C 89 E6 48 8D B8 ? ? ? ?"}.add(11).read()>();
    }
};
