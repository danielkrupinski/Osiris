#pragma once

#include <MemoryPatterns/PatternTypes/HostageServicesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct HostageServicesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToCarriedHostage, CodePattern{"07 C7 47 ? FF FF FF FF C3"}.add(3).read()>();
    }
};
