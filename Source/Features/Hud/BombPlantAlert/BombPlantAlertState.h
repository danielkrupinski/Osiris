#pragma once

#include <CS2/Panorama/PanelHandle.h>

struct BombPlantAlertState {
    cs2::PanelHandle containerPanelHandle{};
    cs2::PanelHandle bombsiteAIconPanelHandle{};
    cs2::PanelHandle bombsiteBIconPanelHandle{};
    cs2::PanelHandle timerPanelHandle{};
};
