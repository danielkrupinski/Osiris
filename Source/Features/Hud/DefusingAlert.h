#pragma once

#include <CS2/Constants/PanelIDs.h>

#include <FeatureHelpers/GlobalVarsProvider.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <Helpers/PlantedC4Provider.h>
#include <Helpers/HudProvider.h>

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
    DefusingAlert(PlantedC4Provider plantedC4Provider, HudProvider hudProvider, GlobalVarsProvider globalVarsProvider) noexcept
        : plantedC4Provider{ plantedC4Provider }
        , hudProvider{ hudProvider }
        , globalVarsProvider{ globalVarsProvider }
    {
    }

    void run() noexcept
    {
        if (!isEnabled())
            return;

        updatePanelHandles();
      
        if (!globalVarsProvider || !globalVarsProvider.getGlobalVars())
            return;

        const PlantedC4 bomb{ plantedC4Provider.getPlantedC4() };
        if (!bomb || !bomb.isBeingDefused()) {
            hideDefusingAlert();
            return;
        }

        const auto timeToEnd = bomb.getTimeToDefuseEnd(globalVarsProvider.getGlobalVars()->curtime);
        if (timeToEnd > 0.0f) {
            if (const auto defusingAlertContainer = defusingAlertContainerPanel.get())
                defusingAlertContainer.setVisible(true);

            if (const auto defusingTimer = defusingTimerPanel.get()) {
                PanoramaLabel{ static_cast<cs2::CLabel*>(defusingTimer.getClientPanel()) }.setTextInternal(DefusingCountdownStringBuilder{}(timeToEnd), 0, true);
                if (const auto style = defusingTimer.getStyle())
                    style.setSimpleForegroundColor(getTimerColor(bomb));
            }
        } else {
            hideDefusingAlert();
        }
    }

    ~DefusingAlert() noexcept
    {
        if (defusingAlertContainerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(defusingAlertContainerPanel.getHandle());
    }
    
private:
    [[nodiscard]] static cs2::Color getTimerColor(PlantedC4 bomb) noexcept
    {
        if (const auto canDefuse = bomb.canBeDefused(); canDefuse.has_value())
            return *canDefuse ? cs2::Color{ 0, 180, 0, 255 } : cs2::Color{ 255, 0, 0, 255 };
        else
            return cs2::Color{ 255, 255, 255, 255 };
    }

    void hideDefusingAlert() const noexcept
    {
        if (const auto defusingAlertContainer = defusingAlertContainerPanel.get())
            defusingAlertContainer.setVisible(false);
    }

    void updatePanelHandles() noexcept
    {
        if (defusingTimerPanel.get())
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
        defusingAlertContainerPanel = defusingAlertContainer;
        defusingTimerPanel = defusingTimer; 
    }

    void onDisable() const noexcept
    {
        hideDefusingAlert();
    }


    friend TogglableFeature;

    PlantedC4Provider plantedC4Provider;
    HudProvider hudProvider;
    GlobalVarsProvider globalVarsProvider;

    PanoramaPanelPointer defusingAlertContainerPanel;
    PanoramaPanelPointer defusingTimerPanel;
};
