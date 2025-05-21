#pragma once

#include <MemoryPatterns/PatternTypes/PortraitWorldPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PortraitWorldPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorldEntities, CodePattern{"4C 63 6F ? 48 89 4D"}.add(3).read()>()
            .template addPattern<OffsetToPortraitWorldMapEntities, CodePattern{"4C 63 63 ? 4C 01"}.add(3).read()>();
    }
};
