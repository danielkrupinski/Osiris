#pragma once

#include <array>

#include <CS2/Panorama/PanelHandle.h>

struct PostRoundTimerState {
    cs2::PanelHandle countdownContainerPanelHandle;
    std::array<char, 10> timerText;
};
