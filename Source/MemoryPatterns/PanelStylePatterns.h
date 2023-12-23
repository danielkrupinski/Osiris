#pragma once

#include <CS2/Classes/Panorama.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PanelStylePatterns {
    [[nodiscard]] cs2::CPanelStyle::SetProperty* setProperty() const noexcept;
    [[nodiscard]] cs2::CPanelStyle::StylePropertySymbols* stylePropertiesSymbols() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& panoramaPatternFinder;
};
