#pragma once

#include "BombTimer.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"

#include <Helpers/HudProvider.h>
#include <Helpers/PlantedC4Provider.h>

struct HudFeatures {
    HudFeatures(PlantedC4Provider plantedC4Provider, HudProvider hudProvider) noexcept
        : bombTimer{ plantedC4Provider, hudProvider }
        , defusingAlert{ plantedC4Provider, hudProvider }
        , killfeedPreserver{ hudProvider }
    {
    }

    BombTimer bombTimer;
    DefusingAlert defusingAlert;
    KillfeedPreserver killfeedPreserver;
};
