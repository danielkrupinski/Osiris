#pragma once

#include <CS2/Classes/Panorama.h>
#include "Implementation/PanoramaImagePanelImpl.h"

struct PanoramaImagePanel {
    explicit PanoramaImagePanel(cs2::CImagePanel* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    [[nodiscard]] static cs2::CImagePanel* create(const char* id, cs2::CUIPanel* parent) noexcept
    {
        if (!PanoramaImagePanelImpl::instance().constructor || !PanoramaImagePanelImpl::instance().size)
            return nullptr;

        const auto memory{static_cast<cs2::CImagePanel*>(MemAlloc::allocate(*PanoramaImagePanelImpl::instance().size))};
        if (memory)
            PanoramaImagePanelImpl::instance().constructor(memory, parent->clientPanel, id);
        return memory;
    }

    void setImage(const char* imageUrl) const noexcept
    {
        if (PanoramaImagePanelImpl::instance().setImage)
            PanoramaImagePanelImpl::instance().setImage(thisptr, imageUrl);
    }

private:
    cs2::CImagePanel* thisptr;
};
