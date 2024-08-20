#pragma once

#include <CS2/Panorama/PanelHandle.h>

struct PostRoundTimerState {
    bool enabled{false};

    cs2::PanelHandle countdownContainerPanelHandle;
};
