#pragma once

#include <MemoryPatterns/PatternTypes/CvarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct CvarPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<CvarPointer, CodePattern{"53 4C 8D 65 ? 48 83 EC ? 48 8D 05 ? ? ? ?"}.add(12).abs()>();
    }

    [[nodiscard]] static consteval auto addTier0Patterns(auto tier0Patterns) noexcept
    {
        return tier0Patterns
            .template addPattern<OffsetToConVarList, CodePattern{"74 ? 0F B7 F6 48 C1 E6 04 48 03 77 ?"}.add(12).read()>();
    }
};
