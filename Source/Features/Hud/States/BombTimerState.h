#pragma once

#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/PanoramaPanelPointer.h>

struct BombTimerState {
    bool enabled{false};

    PanoramaPanelPointer scoreAndTimeAndBombPanel;
    PanoramaPanelPointer bombStatusPanel;

    PanoramaPanelPointer invisiblePanel;
    PanoramaPanelPointer bombTimerContainerPanel;
    PanoramaPanelPointer bombSiteIconPanel;
    PanoramaPanelPointer bombTimerPanel;

    ~BombTimerState() noexcept
    {
        restoreBombStatusPanel();

        if (invisiblePanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(invisiblePanel.getHandle());

        if (bombTimerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(bombTimerPanel.getHandle());

        if (bombTimerContainerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(bombTimerContainerPanel.getHandle());
    }

    void restoreBombStatusPanel() const noexcept
    {
        if (const auto bombStatus = bombStatusPanel.get()) {
            if (const auto scoreAndTimeAndBomb = scoreAndTimeAndBombPanel.get())
                bombStatus.setParent(scoreAndTimeAndBomb);
        }
    }
};
