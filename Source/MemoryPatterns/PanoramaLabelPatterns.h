#pragma once

#include <CS2/Classes/Panorama.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PanoramaLabelPatterns {
    [[nodiscard]] cs2::CLabel::setTextInternal setTextInternal() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
