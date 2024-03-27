#pragma once

#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/PanoramaPanelPointer.h>

#include <FeatureHelpers/TogglableFeature.h>
#include <GameClasses/PanoramaLabel.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <MemoryPatterns/ClientPatterns.h>

#include <GameClasses/PlantedC4.h>
#include <GameClasses/PanoramaImagePanel.h>

#include <Utils/StringBuilder.h>

#include <CS2/Constants/PanelIDs.h>

#include <Helpers/HudProvider.h>

#include "States/BombTimerState.h"

class BombTimer : public TogglableFeature<BombTimer> {
public:
    BombTimer(BombTimerState& state, HookDependencies& hookDependencies, HudProvider hudProvider) noexcept
        : TogglableFeature{state.enabled}
        , state{state}
        , hookDependencies{hookDependencies}
        , hudProvider{hudProvider}
    {
    }

    void run() noexcept
    {
        if (!isEnabled())
            return;

        updatePanelHandles();
        hideBombStatusPanel();

        if (!hookDependencies.requestDependency<PlantedC4>() || !hookDependencies.requestDependency<CurTime>()) {
            hideBombTimerPanel();
            return;
        }

        const PlantedC4 bomb{hookDependencies.getDependency<PlantedC4>()};

        if (const auto timeToExplosion = bomb.getTimeToExplosion(hookDependencies.getDependency<CurTime>()); timeToExplosion > 0.0f) {
            showBombTimerPanel(bomb.getBombSiteIconUrl(), timeToExplosion);
        } else {
            restorePanels();
        }
    }

private:
    void onDisable() const noexcept
    {
        restorePanels();
    }

    friend TogglableFeature;

    void restorePanels() const noexcept
    {
        hideBombTimerPanel();
        state.restoreBombStatusPanel();
    }

    void hideBombTimerPanel() const noexcept
    {
        if (const auto bombTimerContainer = state.bombTimerContainerPanel.get())
            bombTimerContainer.setVisible(false);
    }

    void showBombTimerPanel(const char* bombsiteIconUrl, float timeToExplosion) const noexcept
    {
        if (const auto bombTimerContainer = state.bombTimerContainerPanel.get())
            bombTimerContainer.setVisible(true);

        if (bombsiteIconUrl) {
            if (const auto bombSiteIcon = state.bombSiteIconPanel.get())
                PanoramaImagePanel{ static_cast<cs2::CImagePanel*>(bombSiteIcon.getClientPanel()) }.setImageSvg(bombsiteIconUrl);
        }

        if (const auto bombTimer = state.bombTimerPanel.get()) {
            StringBuilderStorage<10> storage;
            StringBuilder builder = storage.builder();
            builder.put(static_cast<int>(timeToExplosion), '.', static_cast<int>(timeToExplosion * 10) % 10);
            PanoramaLabel{ static_cast<cs2::CLabel*>(bombTimer.getClientPanel()) }.setTextInternal(builder.cstring(), 0, true);
        }
    }

    void updatePanelHandles() noexcept
    {
        if (state.scoreAndTimeAndBombPanel.get())
            return;
 
        const auto hudTeamCounter = hudProvider.findChildInLayoutFile(cs2::HudTeamCounter);
        if (!hudTeamCounter)
            return;

        const auto scoreAndTimeAndBomb = hudTeamCounter.findChildInLayoutFile(cs2::ScoreAndTimeAndBomb);
        if (!scoreAndTimeAndBomb)
            return;

        state.scoreAndTimeAndBombPanel = scoreAndTimeAndBomb;

        const auto bombStatus = scoreAndTimeAndBomb.findChildInLayoutFile("BombStatus");
        if (!bombStatus)
            return;
        
        state.bombStatusPanel = bombStatus;

        PanoramaUiEngine::runScript(scoreAndTimeAndBomb,
            "$.CreatePanel('Panel', $.GetContextPanel().FindChildTraverse('ScoreAndTimeAndBomb'), 'InvisiblePanel');", "", 0);
        const auto invisiblePanelPtr = scoreAndTimeAndBomb.findChildInLayoutFile("InvisiblePanel");
        if (!invisiblePanelPtr)
            return;

        invisiblePanelPtr.setVisible(false);
        state.invisiblePanel = invisiblePanelPtr;

        PanoramaUiEngine::runScript(hudTeamCounter,
R"(
(function() {
  var bombTimerContainer = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile('ScoreAndTimeAndBomb'), 'BombTimerContainer', {
    style: 'flow-children: right; height: 27px; width: 100%; margin-right: 7px; margin-left: 7px;'
  });

  $.CreatePanel('Image', bombTimerContainer, 'BombSiteIcon', {
    style: "width: 22px; height: 22px; vertical-align: center;"
  });

  $.CreatePanel('Label', bombTimerContainer, 'BombTimer', {
    class: 'additive stratum-bold-tf',
    style: 'width: fill-parent-flow(1.0); font-size: 22px; color: white; vertical-align: center; margin-top: 2px; text-align: center;'
  });
})();
)"
        , "", 0);

        const auto bombTimerContainer = scoreAndTimeAndBomb.findChildInLayoutFile("BombTimerContainer");
        if (!bombTimerContainer)
            return;

        state.bombTimerContainerPanel = bombTimerContainer;
        bombTimerContainer.setVisible(false);

        if (const auto bombSiteIcon = bombTimerContainer.findChildInLayoutFile("BombSiteIcon"))
            state.bombSiteIconPanel = bombSiteIcon;

        if (const auto bombTimer = bombTimerContainer.findChildInLayoutFile("BombTimer"))
            state.bombTimerPanel = bombTimer;
    }

    void hideBombStatusPanel() const noexcept
    {
        if (const auto bombStatus = state.bombStatusPanel.get()) {
            if (const auto invisible = state.invisiblePanel.get())
                bombStatus.setParent(invisible);
        }
    }

    BombTimerState& state;
    HookDependencies& hookDependencies;
    HudProvider hudProvider;
};
