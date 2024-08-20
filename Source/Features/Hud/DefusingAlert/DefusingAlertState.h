#pragma once

#include <CS2/Panorama/PanelHandle.h>

struct DefusingAlertState {
    bool enabled{false};

    cs2::PanelHandle defusingAlertContainerPanelHandle;
    cs2::PanelHandle defusingTimerPanelHandle;
};
