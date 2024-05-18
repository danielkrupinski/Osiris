#pragma once

#include <GameClasses/OffsetTypes/TopLevelWindowOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct TopLevelWindowPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] UiScaleFactorOffset uiScaleFactorOffset() const noexcept
    {
        return patternFinders.panoramaPatternFinder("F3 0F 11 49 ? 48 8B F9"_pat).add(4).template readOffset<UiScaleFactorOffset>();
    }
};
