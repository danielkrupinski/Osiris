#pragma once

#include <Utils/Lvalue.h>

#include "DefusingAlertCondition.h"
#include "DefusingAlertPanel.h"
#include "DefusingAlertState.h"
#include "DefusingCountdownTextPanel.h"

template <typename HookContext>
class DefusingAlertContext {
public:
    [[nodiscard]] decltype(auto) defusingAlertContainerPanel() const noexcept
    {
        if (auto&& panel = context.panels().getPanelFromHandle(_state.defusingAlertContainerPanel.handle))
            return utils::lvalue<decltype(panel)>(panel);

        updatePanelHandles();
        return context.panels().getPanelFromHandle(_state.defusingAlertContainerPanel.handle);
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
        return _state;
    }

    [[nodiscard]] auto defusingCountdownTextPanel() const noexcept
    {
        return DefusingCountdownTextPanel{context.panels().getPanelFromHandle(_state.defusingTimerPanel.handle).clientPanel().template as<PanoramaLabel>()};
    }

    HookContext& context;
    DefusingAlertState& _state;

private:
    void updatePanelHandles() const noexcept
    {
        if (_state.defusingTimerPanel.get())
            return;

        auto&& hudTeamCounter = context.hud().hudTeamCounter();
        if (!hudTeamCounter)
            return;

        PanoramaUiEngine::runScript(hudTeamCounter,
            R"(
(function() {
  var defusingAlertContainer = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile('ScoreAndTimeAndBomb'), 'DefusingAlertContainer', {
    style: 'y: 100px; width: 100%; height: 35px; border-radius: 3px; world-blur: hudWorldBlur; background-image: url( "s2r://panorama/images/backgrounds/bluedots_large_png.vtex"); background-size: auto 390px; background-img-opacity: 0.04; margin: 0px 2px; background-color: #0000007f;'
  });

  $.CreatePanel('Image', defusingAlertContainer , '', {
    src: "s2r://panorama/images/icons/equipment/defuser.vsvg",
    style: "x: 10px; width: 25px; height: 25px; vertical-align: center; wash-color: rgb(119, 221, 255);"
});
  $.CreatePanel('Label', defusingAlertContainer , 'DefusingAlertTimer', {
    class: 'additive stratum-bold-tf',
    style: 'x: 42px; font-size: 22px; vertical-align: center; margin-top: 2px; color: white;',
    text: '5.0'
  });
})();
)"
, "", 0);

        const auto defusingAlertContainer = hudTeamCounter.findChildInLayoutFile("DefusingAlertContainer");
        if (!defusingAlertContainer)
            return;

        const auto defusingTimer = defusingAlertContainer.findChildInLayoutFile("DefusingAlertTimer");
        if (!defusingTimer)
            return;

        defusingAlertContainer.setVisible(false);
        _state.defusingAlertContainerPanel.handle = defusingAlertContainer.getHandle();
        _state.defusingTimerPanel.handle = defusingTimer.getHandle();
    }
};
