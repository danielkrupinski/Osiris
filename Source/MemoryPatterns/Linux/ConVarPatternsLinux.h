#pragma once

#include <MemoryPatterns/PatternTypes/ConVarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct ConVarPatterns {
    [[nodiscard]] static consteval auto addTier0Patterns(auto tier0Patterns) noexcept
    {
        return tier0Patterns
            .template addPattern<OffsetToConVarValueType, CodePattern{"? C1 E0 ? 83 C0 ? 48 98 ? 8D 04 ? 48 39 C7 74"}.read()>()
            .template addPattern<OffsetToConVarValue, CodePattern{"F6 49 89 ? 48 8D 40 ?"}.add(7).read()>();
    }
};
