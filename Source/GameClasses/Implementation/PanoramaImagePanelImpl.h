#pragma once

#include <CS2/Classes/Panorama.h>
#include <MemoryPatterns/PanoramaImagePanelPatterns.h>

struct PanoramaImagePanelImpl {
    explicit PanoramaImagePanelImpl(const PanoramaImagePanelPatterns& panoramaImagePanelPatterns) noexcept
        : setImage{panoramaImagePanelPatterns.setImage()}
    {
    }

    [[nodiscard]] static const PanoramaImagePanelImpl& instance() noexcept;

    cs2::CImagePanel::setImage setImage;
};
