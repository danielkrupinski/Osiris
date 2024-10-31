#pragma once

#include <MemoryPatterns/PatternTypes/BaseModelEntityPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct BaseModelEntityPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGlowProperty, CodePattern{"E6 48 8D B8 ? ? ? ? E8 ? ? ? ? 49"}.add(4).read()>();
    }
};
