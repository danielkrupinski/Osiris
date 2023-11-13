#pragma once

#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/PanoramaPanelPointer.h>

#include <FeatureHelpers/GlobalVarsProvider.h>
#include <FeatureHelpers/Hud/BombTimerHelpers.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <GameClasses/PanoramaLabel.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <MemoryPatterns/ClientPatterns.h>

#include <GameClasses/PlantedC4.h>
#include <GameClasses/PanoramaImagePanel.h>

#include <Utils/StringBuilder.h>
#include <Helpers/PlantedC4Provider.h>

#include <CS2/Constants/PanelIDs.h>

#include <Helpers/HudProvider.h>

class BombTimer : public TogglableFeature<BombTimer> {
public:
    void run(const BombTimerHelpers& params) noexcept
    {
        if (!isEnabled())
            return;

        updatePanelHandles(params.hudProvider);
        hideBombStatusPanel();

        const PlantedC4 bomb{ params.plantedC4Provider.getPlantedC4() };

        if (!bomb || !params.globalVarsProvider || !params.globalVarsProvider.getGlobalVars()) {
            hideBombTimerPanel();
            return;
        }

        if (const auto timeToExplosion = bomb.getTimeToExplosion(params.globalVarsProvider.getGlobalVars()->curtime); timeToExplosion > 0.0f) {
            showBombTimerPanel(bomb.getBombSiteIconUrl(), timeToExplosion);
        } else {
            restorePanels();
        }
    }

    ~BombTimer() noexcept
    {
        restoreBombStatusPanel();

        if (invisiblePanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(invisiblePanel.getHandle());

        if (bombTimerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(bombTimerPanel.getHandle());

        if (bombTimerContainerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(bombTimerContainerPanel.getHandle());
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
        restoreBombStatusPanel();
    }

    void hideBombTimerPanel() const noexcept
    {
        if (const auto bombTimerContainer = bombTimerContainerPanel.get())
            bombTimerContainer.setVisible(false);
    }

    void showBombTimerPanel(const char* bombsiteIconUrl, float timeToExplosion) const noexcept
    {
        if (const auto bombTimerContainer = bombTimerContainerPanel.get())
            bombTimerContainer.setVisible(true);

        if (bombsiteIconUrl) {
            if (const auto bombSiteIcon = bombSiteIconPanel.get())
                PanoramaImagePanel{ static_cast<cs2::CImagePanel*>(bombSiteIcon.getClientPanel()) }.setImage(bombsiteIconUrl);
        }

        if (const auto bombTimer = bombTimerPanel.get()) {
            StringBuilderStorage<10> storage;
            StringBuilder builder = storage.builder();
            builder.put(static_cast<int>(timeToExplosion), '.', static_cast<int>(timeToExplosion * 10) % 10);
            PanoramaLabel{ static_cast<cs2::CLabel*>(bombTimer.getClientPanel()) }.setTextInternal(builder.cstring(), 0, true);
        }
    }

    void updatePanelHandles(HudProvider hudProvider) noexcept
    {
        if (scoreAndTimeAndBombPanel.get())
            return;
 
        const auto hudTeamCounter = hudProvider.findChildInLayoutFile(cs2::HudTeamCounter);
        if (!hudTeamCounter)
            return;

        const auto scoreAndTimeAndBomb = hudTeamCounter.findChildInLayoutFile(cs2::ScoreAndTimeAndBomb);
        if (!scoreAndTimeAndBomb)
            return;

        scoreAndTimeAndBombPanel = scoreAndTimeAndBomb;

        const auto bombStatus = scoreAndTimeAndBomb.findChildInLayoutFile("BombStatus");
        if (!bombStatus)
            return;
        
        bombStatusPanel = bombStatus;

        PanoramaUiEngine::runScript(scoreAndTimeAndBomb,
            "$.CreatePanel('Panel', $.GetContextPanel().FindChildTraverse('ScoreAndTimeAndBomb'), 'InvisiblePanel');", "", 0);
        const auto invisiblePanelPtr = scoreAndTimeAndBomb.findChildInLayoutFile("InvisiblePanel");
        if (!invisiblePanelPtr)
            return;

        invisiblePanelPtr.setVisible(false);
        invisiblePanel = invisiblePanelPtr;

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

        bombTimerContainerPanel = bombTimerContainer;
        bombTimerContainer.setVisible(false);

        if (const auto bombSiteIcon = bombTimerContainer.findChildInLayoutFile("BombSiteIcon"))
            bombSiteIconPanel = bombSiteIcon;

        if (const auto bombTimer = bombTimerContainer.findChildInLayoutFile("BombTimer"))
            bombTimerPanel = bombTimer;
    }

    void hideBombStatusPanel() const noexcept
    {
        if (const auto bombStatus = bombStatusPanel.get()) {
            if (const auto invisible = invisiblePanel.get())
                bombStatus.setParent(invisible);
        }
    }

    void restoreBombStatusPanel() const noexcept
    {
        if (const auto bombStatus = bombStatusPanel.get()) {
            if (const auto scoreAndTimeAndBomb = scoreAndTimeAndBombPanel.get())
                bombStatus.setParent(scoreAndTimeAndBomb);
        }
    }

    PanoramaPanelPointer scoreAndTimeAndBombPanel;
    PanoramaPanelPointer bombStatusPanel;

    PanoramaPanelPointer invisiblePanel;
    PanoramaPanelPointer bombTimerContainerPanel;
    PanoramaPanelPointer bombSiteIconPanel;
    PanoramaPanelPointer bombTimerPanel;
};
