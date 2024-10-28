#pragma once

#include <MemoryPatterns/PatternTypes/ConVarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct ConVarPatterns {
    [[nodiscard]] static consteval auto addTier0Patterns(auto tier0Patterns) noexcept
    {
        return tier0Patterns
            .template addPattern<OffsetToConVarValueType, CodePattern{"66 41 89 46 ? 66"}.add(4).read()>()
            .template addPattern<OffsetToConVarValue, CodePattern{"31 F6 48 8D 78 ? 48"}.add(5).read()>();
    }
};
