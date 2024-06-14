#pragma once

#include <CS2/Constants/PanelIDs.h>

#include <FeatureHelpers/FeatureToggle.h>
#include <Helpers/HudProvider.h>
#include <FeatureHelpers/PanelConfigurator.h>

#include "States/DefusingAlertState.h"

struct DefusingCountdownStringBuilder {
    [[nodiscard]] const char* operator()(float timeToDefuseEnd) noexcept
    {
        StringBuilder builder = storage.builder();
        builder.put(static_cast<int>(timeToDefuseEnd), '.', static_cast<int>(timeToDefuseEnd * 10) % 10);
        return builder.cstring();
    }

private:
    StringBuilderStorage<10> storage;
};

struct DefusingAlertToggle : FeatureToggleOff<DefusingAlertToggle> {
    explicit DefusingAlertToggle(DefusingAlertState& state) noexcept
        : state{state}
    {
    }

    DefusingAlertState& state;

    [[nodiscard]] auto& enabledVariable(ToggleMethod) const noexcept
    {
        return state.enabled;
    }

    void onDisable(ToggleMethod) const noexcept
    {
        state.hideDefusingAlert();
    }
};

class DefusingAlert {
public:
    DefusingAlert(DefusingAlertState& state, HookDependencies& hookDependencies, HudOptional hud) noexcept
        : state{state}
        , hookDependencies{hookDependencies}
        , hud{hud}
    {
    }

    void run() noexcept
    {
        if (!state.enabled)
            return;

        updatePanelHandles();
      
        if (!hookDependencies.requestDependencies(HookDependenciesMask{}.set<PlantedC4>()))
            return;

        const PlantedC4 bomb{hookDependencies.getDependency<PlantedC4>()};
        if (!bomb.isBeingDefused()) {
            state.hideDefusingAlert();
            return;
        }

        const auto timeToEnd = bomb.getTimeToDefuseEnd();
        if (timeToEnd && *timeToEnd > 0.0f) {
            if (const auto defusingAlertContainer = state.defusingAlertContainerPanel.get())
                defusingAlertContainer.setVisible(true);

            if (const auto defusingTimer = state.defusingTimerPanel.get()) {
                PanoramaLabel{ static_cast<cs2::CLabel*>(defusingTimer.getClientPanel()) }.setTextInternal(DefusingCountdownStringBuilder{}(*timeToEnd), 0, true);
                if (const auto style = defusingTimer.getStyle())
                    hookDependencies.getDependency<PanelConfigurator>().panelStyle(*style).setSimpleForegroundColor(getTimerColor(bomb));
            }
        } else {
            state.hideDefusingAlert();
        }
    }

private:
    [[nodiscard]] static cs2::Color getTimerColor(PlantedC4 bomb) noexcept
    {
        if (const auto canDefuse = bomb.canBeDefused(); canDefuse.has_value())
            return *canDefuse ? cs2::kColorGreen : cs2::kColorRed;
        return cs2::kColorWhite;
    }

    void updatePanelHandles() noexcept
    {
        if (state.defusingTimerPanel.get())
            return;

        if (!hud)
            return;

        const auto hudTeamCounter = hud->hudTeamCounter();
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
        state.defusingAlertContainerPanel = defusingAlertContainer;
        state.defusingTimerPanel = defusingTimer; 
    }

    DefusingAlertState& state;
    HookDependencies& hookDependencies;
    HudOptional hud;
};
