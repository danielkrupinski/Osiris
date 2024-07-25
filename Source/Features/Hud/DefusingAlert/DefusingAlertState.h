#pragma once

#include <CS2/Classes/Panorama.h>

struct DefusingAlertState {
    bool enabled{false};

    cs2::PanelHandle defusingAlertContainerPanelHandle;
    cs2::PanelHandle defusingTimerPanelHandle;
};
