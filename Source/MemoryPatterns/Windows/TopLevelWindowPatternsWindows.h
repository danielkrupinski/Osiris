#pragma once

#include <MemoryPatterns/PatternTypes/TopLevelWindowPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct TopLevelWindowPatterns {
    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<UiScaleFactorOffset, CodePattern{"F3 0F 11 49 ? 48 8B F9"}.add(4).read()>();
    }
};
