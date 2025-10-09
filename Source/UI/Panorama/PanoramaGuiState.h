#pragma once

#include <CS2/Panorama/PanelHandle.h>
#include "Tabs/VisualsTab/ViewmodelModPreviewPanelState.h"

struct PanoramaGuiState {
    cs2::PanelHandle guiPanelHandle;
    cs2::PanelHandle guiButtonHandle;
    cs2::PanelHandle settingsPanelHandle;
    cs2::PanelHandle modelGlowPreviewPlayerLabelHandleTT;
    cs2::PanelHandle modelGlowPreviewPlayerLabelHandleCT;
    cs2::PanelHandle viewmodelPreviewPanelHandle;
    ViewmodelModPreviewPanelState viewmodelModPreviewPanelState;
};
