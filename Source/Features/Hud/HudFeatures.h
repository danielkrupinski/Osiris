#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include <HookDependencies/HookDependencies.h>

#include "BombTimer/BombTimer.h"
#include "BombTimer/BombTimerContext.h"
#include "BombTimer/BombTimerToggle.h"
#include "DefusingAlert/DefusingAlert.h"
#include "DefusingAlert/DefusingAlertContext.h"
#include "DefusingAlert/DefusingAlertToggle.h"
#include "KillfeedPreserver/KillfeedPreserver.h"
#include "KillfeedPreserver/KillfeedPreserverContext.h"
#include "KillfeedPreserver/KillfeedPreserveToggle.h"

#include "HudFeaturesStates.h"

struct HudFeatures {
    [[nodiscard]] auto bombTimer() const noexcept
    {
        return BombTimer{BombTimerContext{states.bombTimerState, hookDependencies}};
    }

    [[nodiscard]] auto bombTimerToggle() const noexcept
    {
        return BombTimerToggle{BombTimerContext{states.bombTimerState, hookDependencies}};
    }

    [[nodiscard]] auto defusingAlert() const noexcept
    {
        return DefusingAlert{DefusingAlertContext{states.defusingAlertState, hookDependencies}};
    }

    [[nodiscard]] auto defusingAlertToggle() const noexcept
    {
        return DefusingAlertToggle{DefusingAlertContext{states.defusingAlertState, hookDependencies}};
    }
    
    [[nodiscard]] auto killfeedPreserver() const noexcept
    {
        return KillfeedPreserver{KillfeedPreserverContext{states.killfeedPreserverState, hookDependencies}};
    }

    [[nodiscard]] auto killfeedPreserveToggle() const noexcept
    {
        return KillfeedPreserveToggle{KillfeedPreserverContext{states.killfeedPreserverState, hookDependencies}};
    }

    HudFeaturesStates& states;
    FeatureHelpers& helpers;
    HookDependencies& hookDependencies;
};
