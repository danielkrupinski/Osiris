#pragma once

#include <FeatureHelpers/GlobalVarsProvider.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PlantedC4Provider.h>

struct BombTimerHelpers {
    PlantedC4Provider plantedC4Provider;
    HudProvider hudProvider;
    GlobalVarsProvider globalVarsProvider;
};
