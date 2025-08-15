#pragma once

#include <MemoryPatterns/PatternTypes/SliderPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SliderPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SliderSetValueFunction, CodePattern{"FF 50 ? F3 0F 10 45 ? 48 83 C4 ? 48 89 DF 5B 41 5C 41 5D 41 5E 41 5F 5D E9 ? ? ? ?"}.add(26).abs()>();
    }
};
