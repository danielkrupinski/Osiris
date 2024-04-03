#pragma once

#include <GameClasses/OffsetTypes/TopLevelWindowOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct TopLevelWindowPatterns {
    [[nodiscard]] UiScaleFactorOffset uiScaleFactorOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& panoramaPatternFinder;
};
