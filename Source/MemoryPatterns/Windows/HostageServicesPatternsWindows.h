#pragma once

#include <MemoryPatterns/PatternTypes/HostageServicesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct HostageServicesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToCarriedHostage, CodePattern{"8B 48 ? 83 F9 FF 74 ? 4D 85 C0"}.add(2).read()>();
    }
};
