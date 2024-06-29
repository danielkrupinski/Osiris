#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameDependencies/PanoramaImagePanelDeps.h>

struct PanoramaImagePanel {
    explicit PanoramaImagePanel(cs2::CImagePanel* thisptr) noexcept
        : thisptr{thisptr}
    {
    }

    [[nodiscard]] static cs2::CImagePanel* create(const char* id, cs2::CUIPanel* parent) noexcept
    {
        if (!PanoramaImagePanelDeps::instance().constructor || !PanoramaImagePanelDeps::instance().size)
            return nullptr;

        const auto memory{static_cast<cs2::CImagePanel*>(MemAlloc::allocate(*PanoramaImagePanelDeps::instance().size))};
        if (memory)
            PanoramaImagePanelDeps::instance().constructor(memory, parent->clientPanel, id);
        return memory;
    }

    [[nodiscard]] cs2::ImageProperties* getImageProperties() const noexcept
    {
        return PanoramaImagePanelDeps::instance().imagePropertiesOffset.of(thisptr).get();
    }

    void setImageSvg(const char* imageUrl, int textureHeight = -1) const noexcept
    {
        if (thisptr == nullptr)
            return;

        const auto properties{getImageProperties()};
        if (!properties)
            return;

        properties->scale = getUiScaleFactor();
        properties->textureHeight = textureHeight;
        if (PanoramaImagePanelDeps::instance().setImage)
            PanoramaImagePanelDeps::instance().setImage(thisptr, imageUrl, nullptr, properties);
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
