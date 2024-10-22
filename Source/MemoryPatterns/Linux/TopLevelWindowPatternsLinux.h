#pragma once

#include <MemoryPatterns/PatternTypes/TopLevelWindowPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct TopLevelWindowPatterns {
    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<UiScaleFactorOffset, CodePattern{"53 F3 0F 11 47 ?"}.add(5).read()>();
    }
};
