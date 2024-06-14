#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/OffsetTypes/PanoramaImagePanelOffset.h>

struct PanoramaImagePanelDeps {
    template <typename PanoramaImagePanelPatterns>
    explicit PanoramaImagePanelDeps(const PanoramaImagePanelPatterns& panoramaImagePanelPatterns) noexcept
        : setImage{panoramaImagePanelPatterns.setImage()}
        , constructor{panoramaImagePanelPatterns.constructor()}
        , size{panoramaImagePanelPatterns.size()}
        , imagePropertiesOffset{panoramaImagePanelPatterns.imagePropertiesOffset()}
        , offsetToImagePath{panoramaImagePanelPatterns.offsetToImagePath()}
    {
    }

    [[nodiscard]] static const PanoramaImagePanelDeps& instance() noexcept;

    cs2::CImagePanel::SetImage* setImage;
    cs2::CImagePanel::Constructor* constructor;
    std::uint32_t* size;
    ImagePropertiesOffset imagePropertiesOffset;
    OffsetToImagePath offsetToImagePath;
};
