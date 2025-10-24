#pragma once

#include <MemoryPatterns/PatternTypes/PortraitWorldPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PortraitWorldPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorldEntities, CodePattern{"48 89 FB 48 83 EC ? 4C 63 6F ? 48"}.add(10).read()>()
            .template addPattern<OffsetToPortraitWorldMapEntities, CodePattern{"4D 63 A6 ? ? ? ? 45"}.add(3).read()>();
    }
};
