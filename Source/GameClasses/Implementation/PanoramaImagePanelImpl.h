#pragma once

#include <CS2/Classes/Panorama.h>
#include <MemoryPatterns/PanoramaImagePanelPatterns.h>

struct PanoramaImagePanelImpl {
    [[nodiscard]] static const PanoramaImagePanelImpl& instance() noexcept;

    cs2::CImagePanel::setImage setImage{ PanoramaImagePanelPatterns::setImage() };
};
