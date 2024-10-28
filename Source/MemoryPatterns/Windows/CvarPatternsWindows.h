#pragma once

#include <MemoryPatterns/PatternTypes/CvarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct CvarPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<CvarPointer, CodePattern{"48 83 EC ? ? 8B ? ? ? ? ? 48 8D 54 ? ? 4C"}.add(7).abs()>();
    }

    [[nodiscard]] static consteval auto addTier0Patterns(auto tier0Patterns) noexcept
    {
        return tier0Patterns
            .template addPattern<OffsetToConVarList, CodePattern{"74 ? 48 8B 41 ? 44 0F B7"}.add(5).read()>();
    }
};
