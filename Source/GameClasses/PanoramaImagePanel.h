#pragma once

#include <CS2/Classes/Panorama.h>
#include "Implementation/PanoramaImagePanelImpl.h"

struct PanoramaImagePanel {
    explicit PanoramaImagePanel(cs2::CImagePanel* thisptr) noexcept
        : thisptr{thisptr}
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

    [[nodiscard]] cs2::ImageProperties* getImageProperties() const noexcept
    {
        return PanoramaImagePanelImpl::instance().imagePropertiesOffset.of(thisptr).get();
    }

    void setImageSvg(const char* imageUrl, int textureHeight = -1) const noexcept
    {
        const auto properties{getImageProperties()};
        if (!properties)
            return;

        properties->scale = getUiScaleFactor();
        properties->textureHeight = textureHeight;
        if (PanoramaImagePanelImpl::instance().setImage)
            PanoramaImagePanelImpl::instance().setImage(thisptr, imageUrl, nullptr, properties);
    }

private:
    [[nodiscard]] float getUiScaleFactor() const noexcept
    {
        if (const auto parentWindow{PanoramaUiPanel{thisptr->uiPanel}.getParentWindow()})
            return parentWindow.getUiScaleFactor();
        return 1.0f;
    }

    cs2::CImagePanel* thisptr;
};
