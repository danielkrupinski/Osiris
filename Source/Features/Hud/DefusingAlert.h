#pragma once

#include <CS2/Constants/PanelIDs.h>

#include <FeatureHelpers/TogglableFeature.h>
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

class DefusingAlert : public TogglableFeature<DefusingAlert> {
public:
    DefusingAlert(DefusingAlertState& state, HookDependencies& hookDependencies, HudProvider hudProvider, PanelConfigurator panelConfigurator) noexcept
        : TogglableFeature{state.enabled}
        , state{state}
        , hookDependencies{hookDependencies}
        , hudProvider{hudProvider}
        , panelConfigurator{panelConfigurator}
    {
    }

    void run() noexcept
    {
        if (!isEnabled())
            return;

        updatePanelHandles();
      
        if (!hookDependencies.requestDependencies(HookDependenciesMask{}.set<CurTime>().set<PlantedC4>()))
            return;

        const PlantedC4 bomb{hookDependencies.getDependency<PlantedC4>()};
        if (!bomb.isBeingDefused()) {
            hideDefusingAlert();
            return;
        }

        const auto timeToEnd = bomb.getTimeToDefuseEnd(hookDependencies.getDependency<CurTime>());
        if (timeToEnd > 0.0f) {
            if (const auto defusingAlertContainer = state.defusingAlertContainerPanel.get())
                defusingAlertContainer.setVisible(true);

            if (const auto defusingTimer = state.defusingTimerPanel.get()) {
                PanoramaLabel{ static_cast<cs2::CLabel*>(defusingTimer.getClientPanel()) }.setTextInternal(DefusingCountdownStringBuilder{}(timeToEnd), 0, true);
                if (const auto style = defusingTimer.getStyle())
                    panelConfigurator.panelStyle(*style).setSimpleForegroundColor(getTimerColor(bomb));
            }
        } else {
            hideDefusingAlert();
        }
    }

private:
    [[nodiscard]] static cs2::Color getTimerColor(PlantedC4 bomb) noexcept
    {
        constexpr cs2::Color green{0, 180, 0, 255};
        constexpr cs2::Color red{255, 0, 0, 255};
        constexpr cs2::Color white{255, 255, 255, 255};

        if (const auto canDefuse = bomb.canBeDefused(); canDefuse.has_value())
            return *canDefuse ? green : red;
        return white;
    }

    void hideDefusingAlert() const noexcept
    {
        if (const auto defusingAlertContainer = state.defusingAlertContainerPanel.get())
            defusingAlertContainer.setVisible(false);
    }

    void updatePanelHandles() noexcept
    {
        if (state.defusingTimerPanel.get())
            return;

        const auto hudTeamCounter = hudProvider.findChildInLayoutFile(cs2::HudTeamCounter);
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

    void onDisable() const noexcept
    {
        hideDefusingAlert();
    }

    friend TogglableFeature;

    DefusingAlertState& state;
    HookDependencies& hookDependencies;
    HudProvider hudProvider;
    PanelConfigurator panelConfigurator;
};
