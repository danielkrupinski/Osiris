#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelIndex.h>
#include <Utils/DynamicArray.h>

struct PlayerInformationThroughWallsState {
    bool enabled{false};
    bool showPlayerPosition{false};
    bool showPlayerHealth{false};
    bool showPlayerActiveWeapon{false};

    cs2::PanelHandle containerPanelHandle;
    DynamicArray<HudInWorldPanelIndex> panelIndices;
};
