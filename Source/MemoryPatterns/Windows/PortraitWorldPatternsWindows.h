#pragma once

#include <MemoryPatterns/PatternTypes/PortraitWorldPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PortraitWorldPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorldEntities, CodePattern{"48 63 69 ? 45 33 ED"}.add(3).read()>();
    }
};
