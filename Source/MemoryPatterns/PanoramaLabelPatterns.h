#pragma once

#include <CS2/Classes/Panorama.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PanoramaLabelPatterns {
    [[nodiscard]] cs2::CLabel::Constructor* constructor() const noexcept;
    [[nodiscard]] cs2::CLabel::setTextInternal setTextInternal() const noexcept;
    [[nodiscard]] std::uint32_t* size() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
