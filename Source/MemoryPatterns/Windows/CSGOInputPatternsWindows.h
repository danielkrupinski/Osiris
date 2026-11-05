#pragma once

#include <MemoryPatterns/PatternTypes/CSGOInputPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct CSGOInputPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        // TODO: Find code pattern for CSGOInputPointer and OffsetToViewAngles
        return clientPatterns;
    }
};
