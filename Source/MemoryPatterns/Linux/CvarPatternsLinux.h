#pragma once

#include <MemoryPatterns/PatternTypes/CvarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct CvarPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<CvarPointer, CodePattern{"48 89 E5 41 54 53 ? 8D ? ? 48 83 EC ? 48 8D 05 ? ? ? ? 48 8B 38 48 8B 07 FF 50 ? BA"}.add(17).abs()>();
    }

    [[nodiscard]] static consteval auto addTier0Patterns(auto tier0Patterns) noexcept
    {
        return tier0Patterns
            .template addPattern<OffsetToConVarList, CodePattern{"53 ? 45 0F B7 E4"}.add(1).read()>();
    }
};
