#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/OffsetTypes/PanoramaImagePanelOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using ImagePropertiesOffset = PanoramaImagePanelOffset<cs2::ImageProperties, std::int8_t>;

struct PanoramaImagePanelPatterns {
    [[nodiscard]] cs2::CImagePanel::SetImage* setImage() const noexcept;
    [[nodiscard]] cs2::CImagePanel::Constructor* constructor() const noexcept;
    [[nodiscard]] std::uint32_t* size() const noexcept;
    [[nodiscard]] ImagePropertiesOffset imagePropertiesOffset() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
