#pragma once

#include <Utils/Lvalue.h>

#include "PostRoundTimerPanelParams.h"

template <typename HookContext>
struct PostRoundTimerPanelFactory {
    explicit PostRoundTimerPanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    decltype(auto) createCountdownContainerPanel(auto&& parentPanel) const noexcept
    {
        using namespace post_round_timer_panel_params::container_panel_params;

        auto&& panel = hookContext.panelFactory().createPanel(parentPanel).uiPanel();
        panel.setWidth(kWidth);
        panel.setHeight(kHeight);
        return utils::lvalue<decltype(panel)>(panel);
    }

    decltype(auto) createCountdownTextPanel(auto&& containerPanel) const noexcept
    {
        using namespace post_round_timer_panel_params::text_panel_params;

        auto&& panel = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        panel.setWidth(kWidth);
        panel.setMixBlendMode(kMixBlendMode);
        panel.setFont(kFont);
        panel.setTextAlign(kTextAlign);
        return utils::lvalue<decltype(panel)>(panel);
    }

private:
    HookContext& hookContext;
};
