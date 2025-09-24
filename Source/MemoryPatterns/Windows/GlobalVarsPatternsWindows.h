#pragma once

#include <MemoryPatterns/PatternTypes/GlobalVarsPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GlobalVarsPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToFrametime, CodePattern{"44 0F 10 ? ? ? ? ? F3 44 0F 10 ? ? 39"}.add(13).read()>();
    }
};
