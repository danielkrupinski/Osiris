#pragma once

#include <MemoryPatterns/PatternTypes/CSGOInputPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct CSGOInputPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<CSGOInputPointer, CodePattern{"48 8D 05 ? ? ? ? 41 0F B6 D4 44 89 EE 4C 8D 35"}.add(3).abs()>()
            .template addPattern<OffsetToViewAngles, CodePattern{"C7 83 ? ? ? ? 00 00 00 00 49 8B 84 24 ? ? ? ? 48 89 83 ? ? ? ? 41 8B 84 24 ? ? ? ? 89 83"}.add(14).read()>();
    }
};
