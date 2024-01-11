#pragma once

#include <FeatureHelpers/FeatureHelpers.h>

#include "BombTimer.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"
#include "States/HudFeaturesStates.h"

struct HudFeatures {
    [[nodiscard]] BombTimer bombTimer() const noexcept
    {
        return BombTimer{states.bombTimerState, helpers.plantedC4Provider, helpers.hudProvider, helpers.globalVarsProvider};
    }

    [[nodiscard]] DefusingAlert defusingAlert() const noexcept
    {
        return DefusingAlert{
            states.defusingAlertState,
            helpers.plantedC4Provider,
            helpers.hudProvider,
            helpers.globalVarsProvider,
            helpers.panelConfigurator()
        };
    }
    
    [[nodiscard]] KillfeedPreserver killfeedPreserver() const noexcept
    {
        return KillfeedPreserver{states.killfeedPreserverState, helpers.hudProvider, helpers.globalVarsProvider, helpers.gameRules};
    }

    HudFeaturesStates& states;
    FeatureHelpers& helpers;
};
