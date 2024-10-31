#pragma once

#include <MemoryPatterns/PatternTypes/GlowPropertyPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GlowPropertyPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToIsGlowing, CodePattern{"C6 43 ? 00 48 85 C9 74 ? E8"}.add(2).read()>();
    }
};
