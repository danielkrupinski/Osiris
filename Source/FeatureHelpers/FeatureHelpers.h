#pragma once

#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include "HudInWorldPanelContainer.h"
#include "PanelConfigurator.h"
#include "Sound/SoundWatcherState.h"
#include "StylePropertiesSymbolsAndVMTs.h"
#include "StylePropertySymbolMap.h"
#include "ViewToProjectionMatrix.h"
#include "WorldToClipSpaceConverter.h"

struct FeatureHelpers {
    HudInWorldPanelContainer hudInWorldPanelContainer;
    SoundWatcherState soundWatcherState;
};
