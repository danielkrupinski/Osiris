#pragma once

#include <GameClasses/PanoramaUiPanel.h>
#include <Utils/Lvalue.h>

#include "BombSiteIconPanel.h"
#include "BombTimerCondition.h"
#include "BombTimerPanel.h"
#include "BombTimerState.h"
#include "BombTimerTextPanel.h"
#include "GameBombStatusPanel.h"

template <typename HookContext>
struct BombTimerContext {
    [[nodiscard]] bool hasTickingC4() const noexcept
    {
        const auto plantedC4{context.plantedC4()};
        return plantedC4 && plantedC4->isTicking();
    }

    [[nodiscard]] auto tickingC4() const noexcept
    {
        return *context.plantedC4();
    }

    [[nodiscard]] auto bombTimerCondition() const noexcept
    {
        return BombTimerCondition{*this};
    }

    [[nodiscard]] auto gameBombStatusPanel() const noexcept
    {
        return GameBombStatusPanel{*this};
    }

    [[nodiscard]] auto bombTimerPanel() const noexcept
    {
        return BombTimerPanel{*this};
    }

    [[nodiscard]] decltype(auto) bombStatusPanel() const noexcept
    {
        return context.hud().bombStatus();
    }

    [[nodiscard]] decltype(auto) scoreAndTimeAndBombPanel() const noexcept
    {
        return context.hud().scoreAndTimeAndBomb();
    }

    [[nodiscard]] auto invisiblePanel() const noexcept
    {
        if (const auto invisiblePanel{state_.invisiblePanel.get()})
            return invisiblePanel;

        PanoramaUiEngine::runScript(scoreAndTimeAndBombPanel(),
            "$.CreatePanel('Panel', $.GetContextPanel().FindChildTraverse('ScoreAndTimeAndBomb'), 'InvisiblePanel');", "", 0);
        if (const auto invisiblePanelPtr = scoreAndTimeAndBombPanel().findChildInLayoutFile("InvisiblePanel")) {
            invisiblePanelPtr.setVisible(false);
            state_.invisiblePanel.handle = invisiblePanelPtr.getHandle();
        }

        return state_.invisiblePanel.get();
    }

    [[nodiscard]] decltype(auto) bombTimerContainerPanel() const noexcept
    {
        if (auto&& bombTimerContainer = context.panels().getPanelFromHandle(state_.bombTimerContainerPanel.handle))
            return utils::lvalue<decltype(bombTimerContainer)>(bombTimerContainer);

        updatePanelHandles();
        return context.panels().getPanelFromHandle(state_.bombTimerContainerPanel.handle);
    }

    [[nodiscard]] auto bombSiteIconPanel() const noexcept
    {
        return BombSiteIconPanel{context.panels().getPanelFromHandle(state_.bombSiteIconPanel.handle).clientPanel().template as<PanoramaImagePanel>()};
    }

    [[nodiscard]] auto bombTimerTextPanel() const noexcept
    {
        return BombTimerTextPanel{context.panels().getPanelFromHandle(state_.bombTimerPanel.handle).clientPanel().template as<PanoramaLabel>()};
    }

    void updatePanelHandles() const noexcept
    {
        auto&& scoreAndTimeAndBomb = context.hud().scoreAndTimeAndBomb();
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

        state_.bombTimerContainerPanel.handle = bombTimerContainer.getHandle();
        bombTimerContainer.setVisible(false);

        if (const auto bombSiteIcon = bombTimerContainer.findChildInLayoutFile("BombSiteIcon"))
            state_.bombSiteIconPanel.handle = bombSiteIcon.getHandle();

        if (const auto bombTimer = bombTimerContainer.findChildInLayoutFile("BombTimer"))
            state_.bombTimerPanel.handle = bombTimer.getHandle();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return state_;
    }

    BombTimerState& state_;
    HookContext& context;
};
