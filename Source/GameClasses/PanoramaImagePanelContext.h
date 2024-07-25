#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameDependencies/PanoramaImagePanelDeps.h>

template <typename HookContext>
struct PanoramaImagePanelContext {
    PanoramaImagePanelContext(HookContext& hookContext, cs2::CImagePanel* panel) noexcept
        : hookContext{hookContext}
        , panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) uiPanel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(panel->uiPanel);
    }

    [[nodiscard]] const char* getImagePath() const noexcept
    {
        if (auto&& imagePath = PanoramaImagePanelDeps::instance().offsetToImagePath.of(panel).get())
            return imagePath->m_pString;
        return nullptr;
    }

    HookContext& hookContext;
    cs2::CImagePanel* panel;
};
