#pragma once

#include <CS2/Classes/Panorama.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PanelPatterns {
    [[nodiscard]] cs2::CPanel2D::Create* create() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
