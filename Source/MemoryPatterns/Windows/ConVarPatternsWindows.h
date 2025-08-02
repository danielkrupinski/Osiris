#pragma once

#include <MemoryPatterns/PatternTypes/ConVarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct ConVarPatterns {
    [[nodiscard]] static consteval auto addTier0Patterns(auto tier0Patterns) noexcept
    {
        return tier0Patterns
            .template addPattern<OffsetToConVarValueType, CodePattern{"66 41 89 4E ? 0F"}.add(4).read()>()
            .template addPattern<OffsetToConVarValue, CodePattern{"89 44 24 ? 48 8D 48 ?"}.add(7).read()>();
    }
};
