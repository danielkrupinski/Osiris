#pragma once

#include <MemoryPatterns/PatternTypes/SliderPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SliderPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SliderSetValueFunction, CodePattern{"C9 F3 0F 5E CE E8 ? ? ? ?"}.add(6).abs()>();
    }
};
