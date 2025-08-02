#pragma once

#include <MemoryPatterns/PatternTypes/MemAllocPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct MemAllocPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetAllocVirtualMethod, CodePattern{"D6 48 8B 08 48 8B 01 FF 50 ? 48 89 43"}.add(9).read()>();
    }
};
