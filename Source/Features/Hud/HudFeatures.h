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
#include "PostRoundTimer/PostRoundTimer.h"
#include "PostRoundTimer/PostRoundTimerContext.h"
#include "PostRoundTimer/PostRoundTimerToggle.h"

#include "HudFeaturesStates.h"

struct HudFeatures {
    [[nodiscard]] auto bombTimerToggle() const noexcept
    {
        return BombTimerToggle{BombTimerContext{hookDependencies}};
    }

    [[nodiscard]] auto defusingAlert() const noexcept
    {
        return DefusingAlert{DefusingAlertContext{hookDependencies, states.defusingAlertState}};
    }

    [[nodiscard]] auto defusingAlertToggle() const noexcept
    {
        return DefusingAlertToggle{DefusingAlertContext{hookDependencies, states.defusingAlertState}};
    }
    
    [[nodiscard]] auto killfeedPreserver() const noexcept
    {
        return KillfeedPreserver{KillfeedPreserverContext{states.killfeedPreserverState, hookDependencies}};
    }

    [[nodiscard]] auto killfeedPreserveToggle() const noexcept
    {
        return KillfeedPreserveToggle{KillfeedPreserverContext{states.killfeedPreserverState, hookDependencies}};
    }

    [[nodiscard]] decltype(auto) postRoundTimerToggle() const noexcept
    {
        return hookDependencies.make<PostRoundTimerToggle>();
    }

    HudFeaturesStates& states;
    HookDependencies& hookDependencies;
};
