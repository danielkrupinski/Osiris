#pragma once

#include <MemoryPatterns/PatternTypes/RenderComponentPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct RenderComponentPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToSceneObjectUpdaters, CodePattern{"? 7E ? 48 8B 47 ? 48 63 F6 48 8B 34"}.read()>();
    }
};
