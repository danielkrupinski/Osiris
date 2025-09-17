#pragma once

#include <MemoryPatterns/PatternTypes/ConVarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct ConVarPatterns {
    [[nodiscard]] static consteval auto addTier0Patterns(auto tier0Patterns) noexcept
    {
        return tier0Patterns
            .template addPattern<OffsetToConVarValueType, CodePattern{"E4 74 ? 41 0F B7 ? ?"}.add(7).read()>()
            .template addPattern<OffsetToConVarValue, CodePattern{"AF ? 48 8B CE 8D ? ?"}.add(7).read()>();
    }
};
