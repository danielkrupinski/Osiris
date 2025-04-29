#pragma once

#include <MemoryPatterns/PatternTypes/SliderPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SliderPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SliderSetValueFunction, CodePattern{"E8 ? ? ? ? F3 0F 10 45 EC 4C 89 E7 4C 8B 65 F8 C9 E9 ? ? ? ?"}.add(19).abs()>();
    }
};
