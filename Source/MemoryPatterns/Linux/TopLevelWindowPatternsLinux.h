#pragma once

#include <GameClasses/OffsetTypes/TopLevelWindowOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct TopLevelWindowPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] UiScaleFactorOffset uiScaleFactorOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("53 F3 0F 11 47 ?"_pat).add(5).template readOffset<UiScaleFactorOffset>();
    }
};
