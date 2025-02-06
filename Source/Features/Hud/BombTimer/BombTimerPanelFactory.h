#pragma once

#include <Utils/Lvalue.h>

#include "BombTimerPanelParams.h"

template <typename HookContext>
class BombTimerPanelFactory {
public:
    explicit BombTimerPanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    decltype(auto) createContainerPanel(auto&& parentPanel) const noexcept
    {
        using namespace bomb_timer_panel_params::container_panel_params;

        auto&& panel = hookContext.panelFactory().createPanel(parentPanel).uiPanel();
        panel.setFlowChildren(kChildrenFlow);
        panel.setWidth(kWidth);
        panel.setHeight(kHeight);
        panel.setVisible(false);
        return utils::lvalue<decltype(panel)>(panel);
    }

    decltype(auto) createBombSiteIconPanel(auto&& parentPanel) const noexcept
    {
        using namespace bomb_timer_panel_params::bombsite_icon_panel_params;

        auto&& panel = hookContext.panelFactory().createImagePanel(parentPanel).uiPanel();
        panel.setWidth(kWidth);
        panel.setHeight(kHeight);
        panel.setAlign(kAlignment);
        panel.setMargin(kMargin);
        return utils::lvalue<decltype(panel)>(panel);
    }

    decltype(auto) createTimerTextPanel(auto&& parentPanel) const noexcept
    {
        using namespace bomb_timer_panel_params::timer_text_panel_params;

        auto&& panel = hookContext.panelFactory().createLabelPanel(parentPanel).uiPanel();
        panel.setWidth(kWidth);
        panel.setFont(kFont);
        panel.setMixBlendMode(kMixBlendMode);
        panel.setColor(kColor);
        panel.setAlign(kAlignment);
        panel.setTextAlign(kTextAlign);
        return utils::lvalue<decltype(panel)>(panel);
    }

private:
    HookContext& hookContext;
};
