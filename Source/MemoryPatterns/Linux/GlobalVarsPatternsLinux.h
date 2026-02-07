#pragma once

#include <MemoryPatterns/PatternTypes/GlobalVarsPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GlobalVarsPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToFrametime, CodePattern{"28 CF F3 0F 10 40 ?"}.add(6).read()>();
    }
};
