#pragma once

#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelMarginParams.h>
#include <GameClient/Panorama/PanoramaImagePanel.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <Utils/Lvalue.h>

#include "DefusingAlertCondition.h"
#include "DefusingAlertPanel.h"
#include "DefusingAlertPanelParams.h"
#include "DefusingAlertState.h"
#include "DefusingCountdownTextPanel.h"

template <typename HookContext>
class DefusingAlertContext {
public:
    DefusingAlertContext(HookContext& context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] decltype(auto) defusingAlertContainerPanel() const noexcept
    {
        if (auto&& panel = uiEngine().getPanelFromHandle(state().defusingAlertContainerPanelHandle))
            return utils::lvalue<decltype(panel)>(panel);

        updatePanelHandles();
        return uiEngine().getPanelFromHandle(state().defusingAlertContainerPanelHandle);
    }

    [[nodiscard]] auto defusingAlertCondition() const noexcept
    {
        return DefusingAlertCondition{*this};
    }

    [[nodiscard]] auto defusingAlertPanel() const noexcept
    {
        return DefusingAlertPanel{*this};
    }

    [[nodiscard]] auto c4BeingDefused() const noexcept
    {
        return *context.plantedC4();
    }

    [[nodiscard]] bool hasC4BeingDefused() const noexcept
    {
        const auto plantedC4{context.plantedC4()};
        return plantedC4 && plantedC4->isBeingDefused();
    }

    [[nodiscard]] DefusingAlertState& state() const noexcept
    {
        return context.featuresStates().hudFeaturesStates.defusingAlertState;
    }

    [[nodiscard]] auto defusingCountdownTextPanel() const noexcept
    {
        return DefusingCountdownTextPanel{uiEngine().getPanelFromHandle(state().defusingTimerPanelHandle).clientPanel().template as<PanoramaLabel>()};
    }

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return context.config();
    }

private:
    [[nodiscard]] decltype(auto) uiEngine() const noexcept
    {
        return context.template make<PanoramaUiEngine>();
    }

    void updatePanelHandles() const noexcept
    {
        if (uiEngine().getPanelFromHandle(state().defusingTimerPanelHandle))
            return;

        auto&& hudTeamCounter = context.hud().hudTeamCounter();
        if (!hudTeamCounter)
            return;

        uiEngine().runScript(hudTeamCounter,
            R"(
(function() {
  $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile('ScoreAndTimeAndBomb'), 'DefusingAlertContainer', {
    style: 'border-radius: 3px; world-blur: hudWorldBlur; background-image: url( "s2r://panorama/images/backgrounds/bluedots_large_png.vtex"); background-size: auto 390px; background-img-opacity: 0.04; background-color: #0000007f;'
  });
})();
)");

        const auto defusingAlertContainer = hudTeamCounter.findChildInLayoutFile("DefusingAlertContainer");
        if (!defusingAlertContainer)
            return;

        defusingAlertContainer.setVisible(false);
        defusingAlertContainer.setFlowChildren(cs2::k_EFlowRight);
        defusingAlertContainer.setWidth(cs2::CUILength::percent(100));
        defusingAlertContainer.setHeight(cs2::CUILength::pixels(35));
        defusingAlertContainer.setPosition(cs2::CUILength::pixels(0), cs2::CUILength::pixels(100));
        defusingAlertContainer.setMargin(PanelMarginParams{.marginLeft = cs2::CUILength::pixels(1), .marginRight = cs2::CUILength::pixels(1)});

        createIconPanel(defusingAlertContainer);
        auto&& defusingTimer = createTimerPanel(defusingAlertContainer);
        state().defusingAlertContainerPanelHandle = defusingAlertContainer.getHandle();
        state().defusingTimerPanelHandle = defusingTimer.getHandle();
    }

    decltype(auto) createIconPanel(auto&& containerPanel) const noexcept
    {
        using namespace defusing_alert_panel_params::defuse_icon_panel_params;

        auto&& imagePanel = context.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(SvgImageParams{.imageUrl = kImageUrl, .textureHeight = kTextureHeight, .fillColor = kColor});
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setMargin(kMargin);
        return utils::lvalue<decltype(uiPanel)>(uiPanel);
    }

    decltype(auto) createTimerPanel(auto&& containerPanel) const noexcept
    {
        using namespace defusing_alert_panel_params::timer_text_panel_params;

        auto&& panel = context.panelFactory().createLabelPanel(containerPanel).uiPanel();
        panel.setWidth(kWidth);
        panel.setFont(kFont);
        panel.setMixBlendMode(kMixBlendMode);
        panel.setColor(kColor);
        panel.setAlign(kAlignment);
        panel.setTextAlign(kTextAlign);
        return utils::lvalue<decltype(panel)>(panel);
    }

    HookContext& context;
};
