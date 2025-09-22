#pragma once

#include <MemoryPatterns/PatternTypes/MemAllocPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct MemAllocPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetAllocVirtualMethod, CodePattern{"04 00 00 00 49 8B 3C 24 48 8B 07 FF 50 ?"}.add(13).read()>();
    }
};
