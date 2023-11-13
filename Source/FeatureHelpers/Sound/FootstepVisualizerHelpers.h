#pragma once

#include <FeatureHelpers/GlobalVarsProvider.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <Helpers/PanoramaTransformFactory.h>

struct FootstepVisualizerHelpers {
    HudInWorldPanelFactory hudInWorldPanelFactory;
    GlobalVarsProvider globalVarsProvider;
    PanoramaTransformFactory transformFactory;
    WorldToClipSpaceConverter worldtoClipSpaceConverter;
};
