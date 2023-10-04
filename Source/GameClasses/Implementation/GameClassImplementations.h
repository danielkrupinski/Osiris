#pragma once

#include "ClientModeImpl.h"
#include "GameRulesImpl.h"
#include "PanoramaImagePanelImpl.h"
#include "PanoramaLabelImpl.h"
#include "PanoramaUiEngineImpl.h"
#include "PanoramaUiPanelImpl.h"
#include "PlantedC4Impl.h"

struct GameClassImplementations {
    ClientModeImpl clientMode;
    GameRulesImpl gameRules;
    PanoramaImagePanelImpl imagePanel;
    PanoramaLabelImpl panoramaLabel;
    PanoramaUiEngineImpl uiEngine;
    PanoramaUiPanelImpl panoramaUiPanelOffsets;
    PlantedC4Impl plantedC4;
};
