#pragma once

#include <CS2/Panorama/CPanel2D.h>
#include <CS2/Panorama/CUIPanel.h>

template <typename HookContext, typename Context>
struct PanoramaUiPanel;

template <typename HookContext>
struct ClientPanel {
    ClientPanel(HookContext& hookContext, cs2::CPanel2D* panel) noexcept
        : hookContext{hookContext}
        , panel{panel}
    {
    }

    [[nodiscard]] operator cs2::CPanel2D*() const noexcept
    {
        return panel;
    }

    [[nodiscard]] decltype(auto) uiPanel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanelPointer());
    }

    template <template <typename...> typename T>
    [[nodiscard]] decltype(auto) as() const noexcept
    {
        return hookContext.template make<T>(static_cast<T<HookContext>::RawType*>(panel));
    }

private:
    [[nodiscard]] cs2::CUIPanel* uiPanelPointer() const noexcept
    {
        if (panel)
            return panel->uiPanel;
        return nullptr;
    }

    HookContext& hookContext;
    cs2::CPanel2D* panel;
};
