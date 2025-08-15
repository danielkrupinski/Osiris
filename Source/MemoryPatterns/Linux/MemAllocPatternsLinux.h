#pragma once

#include <MemoryPatterns/PatternTypes/MemAllocPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct MemAllocPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetAllocVirtualMethod, CodePattern{"24 48 8B 07 FF 50 ? 41 B8"}.add(6).read()>();
    }
};
