#pragma once

#include <MemoryPatterns/PatternTypes/SliderPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SliderPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SliderSetValueFunction, CodePattern{"CF E8 ? ? ? ? 0F 28 74 24 ? 48 8B 74"}.add(2).abs()>();
    }
};
