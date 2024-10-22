#pragma once

#include <MemoryPatterns/PatternTypes/ConVarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct ConVarPatterns {
    [[nodiscard]] static consteval auto addTier0Patterns(auto tier0Patterns) noexcept
    {
        return tier0Patterns
            .template addPattern<OffsetToConVarValueType, CodePattern{"66 89 4B ? 66 89 43 ? 48"}.add(3).read()>()
            .template addPattern<OffsetToConVarValue, CodePattern{"48 8D 48 ? E8 ? ? ? ? 48 8D"}.add(3).read()>();
    }
};
