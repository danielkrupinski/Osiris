#pragma once

#include <MemoryPatterns/PatternTypes/HostageServicesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct HostageServicesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToCarriedHostage, CodePattern{"C1 44 89 47 ? 48 8B ? ? ? ? ? 41"}.add(4).read()>();
    }
};
