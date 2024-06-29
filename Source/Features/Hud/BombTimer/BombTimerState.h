#pragma once

#include <GameClasses/PanoramaUiEngine.h>
#include <GameDependencies/HudDeps.h>
#include <Helpers/PanoramaPanelPointer.h>

struct BombTimerState {
    bool enabled{false};

    PanoramaPanelPointer invisiblePanel;
    PanoramaPanelPointer bombTimerContainerPanel;
    PanoramaPanelPointer bombSiteIconPanel;
    PanoramaPanelPointer bombTimerPanel;

    ~BombTimerState() noexcept
    {
        HudDeps::instance().bombStatusPanel.get().setParent(HudDeps::instance().scoreAndTimeAndBombPanel.get());

        if (invisiblePanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(invisiblePanel.getHandle());

        if (bombTimerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(bombTimerPanel.getHandle());

        if (bombTimerContainerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(bombTimerContainerPanel.getHandle());
    }
};
