#pragma once

#include <MemoryPatterns/PatternTypes/BaseModelEntityPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct BaseModelEntityPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGlowProperty, CodePattern{"D6 48 8D 88 ? ? ? ?"}.add(4).read()>();
    }
};
