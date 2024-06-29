#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include <HookDependencies/HookDependencies.h>

#include "BombTimer/BombTimer.h"
#include "BombTimer/BombTimerContext.h"
#include "BombTimer/BombTimerToggle.h"
#include "DefusingAlert.h"
#include "KillfeedPreserver.h"
#include "States/HudFeaturesStates.h"

struct HudFeatures {
    [[nodiscard]] auto bombTimer() const noexcept
    {
        return BombTimer{BombTimerContext{states.bombTimerState, hookDependencies}};
    }

    [[nodiscard]] auto bombTimerToggle() const noexcept
    {
        return BombTimerToggle{BombTimerContext{states.bombTimerState, hookDependencies}};
    }

    [[nodiscard]] DefusingAlert defusingAlert() const noexcept
    {
        return DefusingAlert{
            states.defusingAlertState,
            hookDependencies
        };
    }

    [[nodiscard]] DefusingAlertToggle defusingAlertToggle() const noexcept
    {
        return DefusingAlertToggle{states.defusingAlertState};
    }
    
    [[nodiscard]] KillfeedPreserver killfeedPreserver() const noexcept
    {
        return KillfeedPreserver{states.killfeedPreserverState, hookDependencies};
    }

    [[nodiscard]] KillfeedPreserveToggle killfeedPreserveToggle() const noexcept
    {
        return KillfeedPreserveToggle{states.killfeedPreserverState};
    }

    HudFeaturesStates& states;
    FeatureHelpers& helpers;
    HookDependencies& hookDependencies;
};
