#pragma once

#include <Utils/Lvalue.h>

#include "SpectatorListPanelParams.h"

template <typename HookContext>
class SpectatorListPanelFactory {
public:
    explicit SpectatorListPanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) createContainerPanel(auto&& parentPanel) const noexcept
    {
        using namespace spectator_list_panel_params::container_panel_params;

        auto&& panel = hookContext.panelFactory().createPanel(parentPanel, "SpectatorList").uiPanel();
        panel.setWidth(kWidth);
        panel.setHeight(kInitialHeight);
        panel.setPosition(kPositionX, kPositionY);
        panel.setAlign(kAlignment);
        panel.setFlowChildren(kChildrenFlow);
        panel.setBackgroundColor(kBackgroundColor);
        panel.setBorder(kBorderWidth, kBorderColor);
        panel.setBorderRadius(kBorderRadius);
        panel.setZIndex(kZIndex);
        return utils::lvalue<decltype(panel)>(panel);
    }

    [[nodiscard]] decltype(auto) createHeaderPanel(auto&& containerPanel) const noexcept
    {
        using namespace spectator_list_panel_params::header_panel_params;

        auto&& panel = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        panel.setWidth(kWidth);
        panel.setHeight(kHeight);
        panel.setFont(kFont);
        panel.setColor(kColor);
        panel.setTextAlign(kTextAlign);
        panel.setTextShadow(kShadow);
        return utils::lvalue<decltype(panel)>(panel);
    }

    [[nodiscard]] decltype(auto) createNamePanel(auto&& containerPanel) const noexcept
    {
        using namespace spectator_list_panel_params::name_panel_params;

        auto&& panel = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        panel.setWidth(kWidth);
        panel.setHeight(kHeight);
        panel.setFont(kFont);
        panel.setColor(kColor);
        panel.setTextAlign(kTextAlign);
        panel.setTextShadow(kShadow);
        panel.setMargin(kMargin);
        return utils::lvalue<decltype(panel)>(panel);
    }

private:
    HookContext& hookContext;
};
