#pragma once

#include <MemoryPatterns/PatternTypes/GlowPropertyPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GlowPropertyPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToIsGlowing, CodePattern{"43 ? 00 48 85 FF 74 05"}.add(1).read()>();
    }
};
