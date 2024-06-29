#pragma once

#include "BombSiteIconPanel.h"
#include "BombTimerPanel.h"
#include "BombTimerState.h"
#include "BombTimerTextPanel.h"
#include "GameBombStatusPanel.h"

#include <GameClasses/PanoramaUiPanel.h>

struct BombTimerContext {
    [[nodiscard]] decltype(auto) plantedC4() const noexcept
    {
        return dependencies.plantedC4();
    }

    [[nodiscard]] auto gameBombStatusPanel() const noexcept
    {
        return GameBombStatusPanel{*this};
    }

    [[nodiscard]] auto bombTimerPanel() const noexcept
    {
        return BombTimerPanel{*this};
    }

    [[nodiscard]] PanoramaUiPanel bombStatusPanel() const noexcept
    {
        return dependencies.hud().bombStatus();
    }

    [[nodiscard]] PanoramaUiPanel scoreAndTimeAndBombPanel() const noexcept
    {
        return dependencies.hud().scoreAndTimeAndBomb();
    }

    [[nodiscard]] PanoramaUiPanel invisiblePanel() const noexcept
    {
        if (const auto invisiblePanel{state_.invisiblePanel.get()})
            return invisiblePanel;

        PanoramaUiEngine::runScript(scoreAndTimeAndBombPanel(),
            "$.CreatePanel('Panel', $.GetContextPanel().FindChildTraverse('ScoreAndTimeAndBomb'), 'InvisiblePanel');", "", 0);
        if (const auto invisiblePanelPtr = scoreAndTimeAndBombPanel().findChildInLayoutFile("InvisiblePanel")) {
            invisiblePanelPtr.setVisible(false);
            state_.invisiblePanel = invisiblePanelPtr;
        }

        return state_.invisiblePanel.get();
    }

    [[nodiscard]] PanoramaUiPanel bombTimerContainerPanel() const noexcept
    {
        if (const auto bombTimerContainer{state_.bombTimerContainerPanel.get()})
            return bombTimerContainer;

        updatePanelHandles();
        return state_.bombTimerContainerPanel.get();
    }

    [[nodiscard]] BombSiteIconPanel<PanoramaImagePanel> bombSiteIconPanel() const noexcept
    {
        if (const auto bombSiteIcon = state_.bombSiteIconPanel.get())
            return BombSiteIconPanel{PanoramaImagePanel{static_cast<cs2::CImagePanel*>(bombSiteIcon.getClientPanel())}};
        return BombSiteIconPanel{PanoramaImagePanel{nullptr}};
    }

    [[nodiscard]] BombTimerTextPanel<PanoramaLabel> bombTimerTextPanel() const noexcept
    {
        if (const auto bombTimer = state_.bombTimerPanel.get())
            return BombTimerTextPanel{PanoramaLabel{static_cast<cs2::CLabel*>(bombTimer.getClientPanel())}};
        return BombTimerTextPanel{PanoramaLabel{nullptr}};
    }

    void updatePanelHandles() const noexcept
    {
        const auto scoreAndTimeAndBomb = dependencies.hud().scoreAndTimeAndBomb();
        if (!scoreAndTimeAndBomb)
            return;

        PanoramaUiEngine::runScript(scoreAndTimeAndBomb,
            R"(
(function() {
  var bombTimerContainer = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile('ScoreAndTimeAndBomb'), 'BombTimerContainer', {
    style: 'flow-children: right; height: 32px; width: 100%;'
  });

  $.CreatePanel('Image', bombTimerContainer, 'BombSiteIcon', {
    style: "width: 26px; height: 26px; vertical-align: center; margin-left: 5px;"
  });

  $.CreatePanel('Label', bombTimerContainer, 'BombTimer', {
    class: 'additive stratum-bold-tf',
    style: 'width: fill-parent-flow(1.0); font-size: 22px; color: white; vertical-align: center; text-align: center;'
  });
})();
)"
, "", 0);

        const auto bombTimerContainer = scoreAndTimeAndBomb.findChildInLayoutFile("BombTimerContainer");
        if (!bombTimerContainer)
            return;

        state_.bombTimerContainerPanel = bombTimerContainer;
        bombTimerContainer.setVisible(false);

        if (const auto bombSiteIcon = bombTimerContainer.findChildInLayoutFile("BombSiteIcon"))
            state_.bombSiteIconPanel = bombSiteIcon;

        if (const auto bombTimer = bombTimerContainer.findChildInLayoutFile("BombTimer"))
            state_.bombTimerPanel = bombTimer;
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return state_;
    }

    BombTimerState& state_;
    HookDependencies& dependencies;
};
