#pragma once

#include <MemoryPatterns/PatternTypes/PortraitWorldPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PortraitWorldPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorldEntities, CodePattern{"48 63 ? ? 45 33 ED 4D"}.add(3).read()>()
            .template addPattern<OffsetToPortraitWorldMapEntities, CodePattern{"4C 63 ? ? ? ? ? 42 8B 1C"}.add(3).read()>();
    }
};
