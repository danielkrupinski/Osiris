#pragma once

#include "BombPlantAlertPanelParams.h"
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <Utils/Lvalue.h>

template <typename HookContext>
class BombPlantAlertPanelFactory {
public:
    explicit BombPlantAlertPanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    decltype(auto) createContainerPanel(auto&& parentPanel) const
    {
        using namespace bomb_plant_alert_panel_params::container_panel_params;

        auto&& container = hookContext.panelFactory().createPanel(parentPanel, "BombPlantAlert").uiPanel();
        container.setFlowChildren(kChildrenFlow);
        container.setWidth(kWidth);
        container.setHeight(kHeight);
        container.setPosition(kPositionX, kPositionY);
        container.setBorderRadius(kBorderRadius);
        // todo: implement setting below style properties in C++
        hookContext.template make<PanoramaUiEngine>().runScript(parentPanel,
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

    decltype(auto) createBombsiteIconPanel(auto&& containerPanel, const char* imageUrl) const
    {
        using namespace bomb_plant_alert_panel_params::bombsite_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(imageUrl, kTextureHeight);
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setMargin(kMargin);
        return utils::lvalue<decltype(uiPanel)>(uiPanel);
    }

    decltype(auto) createTimerPanel(auto&& containerPanel) const
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
