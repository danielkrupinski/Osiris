#pragma once

#include <cstdint>

#include <CS2/Classes/Panorama.h>
#include <GameClasses/OffsetTypes/PanoramaImagePanelOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PanoramaImagePanelPatterns {
    [[nodiscard]] cs2::CImagePanel::SetImage* setImage() const noexcept;
    [[nodiscard]] cs2::CImagePanel::Constructor* constructor() const noexcept;
    [[nodiscard]] std::uint32_t* size() const noexcept;
    [[nodiscard]] ImagePropertiesOffset imagePropertiesOffset() const noexcept;
    [[nodiscard]] OffsetToImagePath offsetToImagePath() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
