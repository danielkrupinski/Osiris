#pragma once

#include <cstdint>

#include <GameClasses/OffsetTypes/TopLevelWindowOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using UiScaleFactorOffset = TopLevelWindowOffset<cs2::CTopLevelWindow::m_flScaleFactor, std::int8_t>;

struct TopLevelWindowPatterns {
    [[nodiscard]] UiScaleFactorOffset uiScaleFactorOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& panoramaPatternFinder;
};
