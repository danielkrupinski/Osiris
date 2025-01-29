#pragma once

#include <CS2/Panorama/CUIPanel.h>

template <typename HookContext, typename Context>
struct PanoramaUiPanel;

template <typename HookContext>
struct PanoramaUiPanelIterator {
    PanoramaUiPanelIterator(HookContext& hookContext, cs2::CUIPanel** panel) noexcept
        : hookContext{hookContext}
        , panel{panel}
    {
    }

    [[nodiscard]] friend bool operator==(const PanoramaUiPanelIterator& lhs, const PanoramaUiPanelIterator& rhs) noexcept
    {
        return lhs.panel == rhs.panel;
    }

    PanoramaUiPanelIterator& operator++() noexcept
    {
        ++panel;
        return *this;
    }

    [[nodiscard]] auto operator*() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(*panel);
    }

    HookContext& hookContext;
    cs2::CUIPanel** panel;
};
