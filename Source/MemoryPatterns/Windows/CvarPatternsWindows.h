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
            .template addPattern<OffsetToConVarList, CodePattern{"48 8B ? ? 0F B7 ? 48 03 ? ? 8B ? ? ? 8B 46"}.add(3).read()>();
    }
};
