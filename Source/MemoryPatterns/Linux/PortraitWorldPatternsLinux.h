#pragma once

#include <MemoryPatterns/PatternTypes/PortraitWorldPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PortraitWorldPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorldEntities, CodePattern{"48 89 FB 48 83 EC ? 4C 63 6F ? 48"}.add(10).read()>()
            .template addPattern<OffsetToPortraitWorldMapEntities, CodePattern{"A3 ? ? ? ? 8B BB ? ? ? ? 44 8B"}.add(1).read()>();
    }
};
