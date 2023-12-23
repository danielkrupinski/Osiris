#pragma once

#include <CS2/Classes/Panorama.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PanoramaImagePanelPatterns {
    [[nodiscard]] cs2::CImagePanel::setImage setImage() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
