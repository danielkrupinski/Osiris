#pragma once

#include <CS2/Classes/Panorama.h>
#include "Implementation/PanoramaImagePanelImpl.h"

struct PanoramaImagePanel {
    explicit PanoramaImagePanel(cs2::CImagePanel* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    void setImage(const char* imageUrl) const noexcept
    {
        if (PanoramaImagePanelImpl::instance().setImage)
            PanoramaImagePanelImpl::instance().setImage(thisptr, imageUrl);
    }

private:
    cs2::CImagePanel* thisptr;
};
