#pragma once

#include <CS2/Panorama/CImagePanel.h>
#include <GameClasses/OffsetTypes/PanoramaImagePanelOffset.h>

struct PanoramaImagePanelDeps {
    PanoramaImagePanelDeps() = default;

    template <typename PanoramaImagePanelPatterns>
    explicit PanoramaImagePanelDeps(const PanoramaImagePanelPatterns& panoramaImagePanelPatterns) noexcept
        : setImage{panoramaImagePanelPatterns.setImage()}
        , constructor{panoramaImagePanelPatterns.constructor()}
        , size{panoramaImagePanelPatterns.size()}
        , imagePropertiesOffset{panoramaImagePanelPatterns.imagePropertiesOffset()}
        , offsetToImagePath{panoramaImagePanelPatterns.offsetToImagePath()}
    {
    }

    cs2::CImagePanel::SetImage* setImage;
    cs2::CImagePanel::Constructor* constructor;
    std::uint32_t* size;
    ImagePropertiesOffset imagePropertiesOffset;
    OffsetToImagePath offsetToImagePath;
};
