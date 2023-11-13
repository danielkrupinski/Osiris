#pragma once

#include <FeatureHelpers/GlobalVarsProvider.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PlantedC4Provider.h>

struct DefusingAlertHelpers {
    PlantedC4Provider plantedC4Provider;
    HudProvider hudProvider;
    GlobalVarsProvider globalVarsProvider;
};
