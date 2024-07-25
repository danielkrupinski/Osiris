#pragma once

#include <CS2/Classes/Panorama.h>

#include "PanoramaUiEngine.h"

template <typename HookContext>
struct Panels {
    explicit Panels(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] cs2::PanelHandle getPanelHandle(cs2::CUIPanel* panel) noexcept
    {
        return PanoramaUiEngine::getPanelHandle(panel);
    }

    [[nodiscard]] decltype(auto) getPanelFromHandle(cs2::PanelHandle handle) noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(PanoramaUiEngine::getPanelPointer(handle));
    }

    void deletePanelByHandle(cs2::PanelHandle handle) noexcept
    {
        if (handle.isValid())
            PanoramaUiEngine::onDeletePanel(handle);
    }

private:
    HookContext& hookContext;
};
