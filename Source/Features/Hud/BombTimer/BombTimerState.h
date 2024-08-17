#pragma once

#include <CS2/Classes/Panorama.h>

struct BombTimerState {
    bool enabled{false};

    cs2::PanelHandle bombTimerContainerPanelHandle;
    cs2::PanelHandle bombSiteIconPanelHandle;
    cs2::PanelHandle bombTimerPanelHandle;
};
