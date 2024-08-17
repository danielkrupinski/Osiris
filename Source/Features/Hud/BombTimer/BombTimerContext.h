#pragma once

#include <GameClasses/PanoramaImagePanel.h>
#include <GameClasses/PanoramaLabel.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <Utils/Lvalue.h>

#include "BombSiteIconPanel.h"
#include "BombTimerCondition.h"
#include "BombTimerPanel.h"
#include "BombTimerState.h"
#include "BombTimerTextPanel.h"

template <typename HookContext>
struct BombTimerContext {
    BombTimerContext(HookContext& context) noexcept
        : _context{context}
    {
    }

    [[nodiscard]] bool hasTickingC4() const noexcept
    {
        const auto plantedC4{_context.plantedC4()};
        return plantedC4 && plantedC4->isTicking();
    }

    [[nodiscard]] auto tickingC4() const noexcept
    {
        return *_context.plantedC4();
    }

    [[nodiscard]] auto bombTimerCondition() const noexcept
    {
        return BombTimerCondition{*this};
    }

    [[nodiscard]] auto bombTimerPanel() const noexcept
    {
        return BombTimerPanel{*this};
    }

    [[nodiscard]] decltype(auto) bombPlantedPanel() const noexcept
    {
        return _context.hud().bombPlantedPanel();
    }

    [[nodiscard]] decltype(auto) bombTimerContainerPanel() const noexcept
    {
        if (auto&& bombTimerContainer = _context.panels().getPanelFromHandle(state().bombTimerContainerPanelHandle))
            return utils::lvalue<decltype(bombTimerContainer)>(bombTimerContainer);

        updatePanelHandles();
        return _context.panels().getPanelFromHandle(state().bombTimerContainerPanelHandle);
    }

    [[nodiscard]] auto bombSiteIconPanel() const noexcept
    {
        return BombSiteIconPanel{_context.panels().getPanelFromHandle(state().bombSiteIconPanelHandle).clientPanel().template as<PanoramaImagePanel>()};
    }

    [[nodiscard]] auto bombTimerTextPanel() const noexcept
    {
        return BombTimerTextPanel{_context.panels().getPanelFromHandle(state().bombTimerPanelHandle).clientPanel().template as<PanoramaLabel>()};
    }

    void updatePanelHandles() const noexcept
    {
        auto&& scoreAndTimeAndBomb = _context.hud().scoreAndTimeAndBomb();
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

        state().bombTimerContainerPanelHandle = bombTimerContainer.getHandle();
        bombTimerContainer.setVisible(false);

        if (const auto bombSiteIcon = bombTimerContainer.findChildInLayoutFile("BombSiteIcon"))
            state().bombSiteIconPanelHandle = bombSiteIcon.getHandle();

        if (const auto bombTimer = bombTimerContainer.findChildInLayoutFile("BombTimer"))
            state().bombTimerPanelHandle = bombTimer.getHandle();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return _context.featuresStates().hudFeaturesStates.bombTimerState;
    }

private:
    HookContext& _context;
};
