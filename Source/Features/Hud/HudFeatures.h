#pragma once

#include <FeatureHelpers/FeatureHelpers.h>

#include "BombTimer.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"
#include "States/HudFeaturesStates.h"

struct HudFeatures {
    [[nodiscard]] BombTimer bombTimer() const noexcept
    {
        return BombTimer{states.bombTimerState, hookDependencies, helpers.hudProvider};
    }

    [[nodiscard]] DefusingAlert defusingAlert() const noexcept
    {
        return DefusingAlert{
            states.defusingAlertState,
            hookDependencies,
            helpers.hudProvider,
            helpers.panelConfigurator()
        };
    }
    
    [[nodiscard]] KillfeedPreserver killfeedPreserver() const noexcept
    {
        return KillfeedPreserver{states.killfeedPreserverState, hookDependencies, helpers.hudProvider, helpers.gameRules};
    }

    HudFeaturesStates& states;
    FeatureHelpers& helpers;
    HookDependencies& hookDependencies;
};
