#pragma once

#include "DefusingAlertPanel.h"
#include "DefusingAlertState.h"
#include "DefusingCountdownTextPanel.h"

class DefusingAlertContext {
public:
    [[nodiscard]] PanoramaUiPanel defusingAlertContainerPanel() const noexcept
    {
        if (const auto panel{_state.defusingAlertContainerPanel.get()})
            return panel;

        updatePanelHandles();
        return _state.defusingAlertContainerPanel.get();
    }

    [[nodiscard]] auto defusingAlertPanel() const noexcept
    {
        return DefusingAlertPanel{*this};
    }

    [[nodiscard]] decltype(auto) plantedC4() const noexcept
    {
        return dependencies.plantedC4();
    }

    [[nodiscard]] DefusingAlertState& state() const noexcept
    {
        return _state;
    }

    [[nodiscard]] auto defusingCountdownTextPanel() const noexcept
    {
        if (const auto countdownPanel = _state.defusingTimerPanel.get())
            return DefusingCountdownTextPanel{PanoramaLabel{static_cast<cs2::CLabel*>(countdownPanel.getClientPanel())}};
        return DefusingCountdownTextPanel{PanoramaLabel{nullptr}};
    }

    DefusingAlertState& _state;
    HookDependencies& dependencies;

private:
    void updatePanelHandles() const noexcept
    {
        if (_state.defusingTimerPanel.get())
            return;

        const auto hudTeamCounter = dependencies.hud().hudTeamCounter();
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
        _state.defusingAlertContainerPanel = defusingAlertContainer;
        _state.defusingTimerPanel = defusingTimer;
    }
};
