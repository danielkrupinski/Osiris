#pragma once

#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/PanoramaPanelPointer.h>

struct DefusingAlertState {
    bool enabled{false};

    PanoramaPanelPointer defusingAlertContainerPanel;
    PanoramaPanelPointer defusingTimerPanel;

    ~DefusingAlertState() noexcept
    {
        if (defusingAlertContainerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(defusingAlertContainerPanel.getHandle());
    }
};
