#pragma once

#include <MemoryPatterns/PatternTypes/C4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct C4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToIsBeingPlanted, CodePattern{"D2 74 ? 44 0F B6 B7 ? ? ? ?"}.add(7).read()>();
    }
};
