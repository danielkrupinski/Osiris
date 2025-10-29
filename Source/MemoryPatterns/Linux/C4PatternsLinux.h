#pragma once

#include <MemoryPatterns/PatternTypes/C4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct C4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToIsBeingPlanted, CodePattern{"? ? ? ? 00 75 ? 48 63 87 ? ? ? ? 48"}.read()>()
            .template addPattern<OffsetToArmingEndTime, CodePattern{"? ? ? ? 00 00 00 00 66 89 ? ? ? ? ? 48 89 83 ? ? ? ? C6"}.read()>();
    }
};
