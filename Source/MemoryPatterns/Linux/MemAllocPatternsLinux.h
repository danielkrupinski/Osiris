#pragma once

#include <MemoryPatterns/PatternTypes/MemAllocPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct MemAllocPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetAllocVirtualMethod, CodePattern{"E6 06 FF 50 ? 48 89 43"}.add(4).read()>();
    }
};
