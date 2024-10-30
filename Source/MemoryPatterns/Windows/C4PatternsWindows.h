#pragma once

#include <MemoryPatterns/PatternTypes/C4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct C4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToIsBeingPlanted, CodePattern{"80 B9 ? ? ? ? 00 74 ? 48 8D 0D"}.add(2).read()>();
    }
};
