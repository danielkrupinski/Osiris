#pragma once

#include "BombPlantAlertPanelParams.h"
#include <GameClient/Panorama/PanoramaImagePanel.h>

template <typename HookContext>
class BombPlantAlertPanelFactory {
public:
    explicit BombPlantAlertPanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    decltype(auto) createContainerPanel() const
    {
        auto&& container = hookContext.panelFactory().createPanel(hookContext.hud().scoreAndTimeAndBomb(), "BombPlantAlert").uiPanel();
        container.setVisible(false);
        container.setFlowChildren(cs2::k_EFlowRight);
        container.setWidth(cs2::CUILength::percent(100));
        container.setHeight(cs2::CUILength::pixels(40));
        container.setPosition(cs2::CUILength::pixels(0), cs2::CUILength::pixels(100));
        container.setBorderRadius(cs2::CUILength::pixels(5));
        // todo: implement setting below style properties in C++
        hookContext.template make<PanoramaUiEngine>().runScript(hookContext.hud().hudTeamCounter(),
            R"(
(function() {
  var panel = $.GetContextPanel().FindChildInLayoutFile('BombPlantAlert');
  panel.style.worldBlur = 'gaussian(2, 2, 2)';
  panel.style.backgroundImage = 'url("s2r://panorama/images/backgrounds/bluedots_large_png.vtex")';
  panel.style.backgroundSize = 'auto 390px';
  panel.style.backgroundImgOpacity = '0.04';
})();
)");
        return utils::lvalue<decltype(container)>(container);
    }

    decltype(auto) createBombsiteIconPanel(auto&& containerPanel, const char* imageUrl) const noexcept
    {
        using namespace bomb_plant_alert_panel_params::bombsite_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(SvgImageParams{.imageUrl = imageUrl, .textureHeight = kTextureHeight});
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setMargin(kMargin);
        return utils::lvalue<decltype(uiPanel)>(uiPanel);
    }

    decltype(auto) createTimerPanel(auto&& containerPanel) const noexcept
    {
        using namespace bomb_plant_alert_panel_params::timer_panel_params;

        auto&& panel = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        panel.setWidth(kWidth);
        panel.setFont(kFont);
        panel.setMixBlendMode(kMixBlendMode);
        panel.setAlign(kAlignment);
        panel.setMargin(kMargin);
        panel.setTextAlign(kTextAlign);
        return utils::lvalue<decltype(panel)>(panel);
    }

private:
    HookContext& hookContext;
};
