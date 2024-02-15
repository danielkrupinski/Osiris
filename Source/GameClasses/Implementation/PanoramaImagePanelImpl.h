#pragma once

#include <CS2/Classes/Panorama.h>
#include <MemoryPatterns/PanoramaImagePanelPatterns.h>

struct PanoramaImagePanelImpl {
    explicit PanoramaImagePanelImpl(const PanoramaImagePanelPatterns& panoramaImagePanelPatterns) noexcept
        : setImage{panoramaImagePanelPatterns.setImage()}
        , constructor{panoramaImagePanelPatterns.constructor()}
        , size{panoramaImagePanelPatterns.size()}
    {
    }

    [[nodiscard]] static const PanoramaImagePanelImpl& instance() noexcept;

    cs2::CImagePanel::setImage setImage;
    cs2::CImagePanel::Constructor* constructor;
    std::uint32_t* size;
};
