#pragma once

#include <array>

#include <CS2/Panorama/PanelHandle.h>

struct BombTimerState {
    cs2::PanelHandle bombTimerContainerPanelHandle;
    cs2::PanelHandle bombSiteIconPanelHandle;
    cs2::PanelHandle bombTimerPanelHandle;
    std::array<char, 10> timerText;
};
