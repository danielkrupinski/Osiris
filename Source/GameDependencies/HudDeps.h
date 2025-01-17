#pragma once

#include <CS2/Panorama/CPanel2D.h>
#include <CS2/Panorama/PanelHandle.h>

struct HudDeps {
    HudDeps() = default;

    cs2::PanelHandle deathNoticesPanelHandle;
    cs2::PanelHandle scoreAndTimeAndBombPanelHandle;
    cs2::PanelHandle bombStatusPanelHandle;
    cs2::PanelHandle bombPlantedPanelHandle;
    cs2::PanelHandle timerTextPanelHandle;
};
