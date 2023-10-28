#pragma once

#include "BombTimer.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"

#include <FeatureHelpers/GlobalVarsProvider.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PlantedC4Provider.h>

struct HudFeatures {
    HudFeatures(PlantedC4Provider plantedC4Provider, HudProvider hudProvider, GlobalVarsProvider globalVarsProvider) noexcept
        : bombTimer{ plantedC4Provider, hudProvider, globalVarsProvider }
        , defusingAlert{ plantedC4Provider, hudProvider, globalVarsProvider }
        , killfeedPreserver{ hudProvider, globalVarsProvider }
    {
    }

    BombTimer bombTimer;
    DefusingAlert defusingAlert;
    KillfeedPreserver killfeedPreserver;
};
