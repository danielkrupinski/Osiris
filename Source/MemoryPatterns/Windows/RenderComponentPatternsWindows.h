#pragma once

#include <MemoryPatterns/PatternTypes/RenderComponentPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct RenderComponentPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToSceneObjectUpdaters, CodePattern{"? 7C 03 33 C0 C3 48 8B 41"}.read()>();
    }
};
