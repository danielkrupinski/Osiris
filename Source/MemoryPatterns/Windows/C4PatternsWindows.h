#pragma once

#include <MemoryPatterns/PatternTypes/C4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct C4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToIsBeingPlanted, CodePattern{"? ? ? ? 00 74 ? F3 0F 10 05 ? ? ? ? C3"}.read()>()
            .template addPattern<OffsetToArmingEndTime, CodePattern{"05 ? ? ? ? F3 0F 11 87 ? ? ? ? 44"}.add(9).read()>();
    }
};
