#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include <HookDependencies/HookDependencies.h>

#include "BombTimer.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"
#include "States/HudFeaturesStates.h"

struct HudFeatures {
    [[nodiscard]] BombTimer bombTimer() const noexcept
    {
        return BombTimer{states.bombTimerState, hookDependencies, helpers.hudProvider};
    }

    [[nodiscard]] BombTimerToggle bombTimerToggle() const noexcept
    {
        return BombTimerToggle{states.bombTimerState};
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

    [[nodiscard]] DefusingAlertToggle defusingAlertToggle() const noexcept
    {
        return DefusingAlertToggle{states.defusingAlertState};
    }
    
    [[nodiscard]] KillfeedPreserver killfeedPreserver() const noexcept
    {
        return KillfeedPreserver{states.killfeedPreserverState, hookDependencies, helpers.hudProvider, hookDependencies.offsets().gameRules.gameRules};
    }

    [[nodiscard]] KillfeedPreserveToggle killfeedPreserveToggle() const noexcept
    {
        return KillfeedPreserveToggle{states.killfeedPreserverState};
    }

    HudFeaturesStates& states;
    FeatureHelpers& helpers;
    HookDependencies& hookDependencies;
};
