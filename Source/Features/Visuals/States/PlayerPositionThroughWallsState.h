#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelIndex.h>
#include <HookDependencies/HookDependenciesMask.h>
#include <Utils/DynamicArray.h>

struct PlayerPositionThroughWallsState {
    bool enabled{false};
    cs2::PanelHandle containerPanelHandle;
    DynamicArray<HudInWorldPanelIndex> panelIndices;
};
