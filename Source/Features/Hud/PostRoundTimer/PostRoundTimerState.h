#pragma once

#include <CS2/Classes/Panorama.h>

struct PostRoundTimerState {
    bool enabled{false};

    cs2::PanelHandle countdownContainerPanelHandle;
};
