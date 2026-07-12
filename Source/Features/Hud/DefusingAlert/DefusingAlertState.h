#pragma once

#include <array>

#include <CS2/Panorama/PanelHandle.h>

struct DefusingAlertState {
    cs2::PanelHandle defusingAlertContainerPanelHandle;
    cs2::PanelHandle defusingTimerPanelHandle;
    std::array<char, 10> timerText;
};
