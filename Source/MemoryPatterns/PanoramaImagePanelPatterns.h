#pragma once

#include <CS2/Classes/Panorama.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PanoramaImagePanelPatterns {
    [[nodiscard]] cs2::CImagePanel::setImage setImage() const noexcept;
    [[nodiscard]] cs2::CImagePanel::Constructor* constructor() const noexcept;
    [[nodiscard]] std::uint32_t* size() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
