#pragma once

#include <MemoryPatterns/PatternTypes/GlowPropertyPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GlowPropertyPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToIsGlowing, CodePattern{"E0 48 8B 7B ? C6 43 ?"}.add(7).read()>();
    }
};
