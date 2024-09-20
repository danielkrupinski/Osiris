#pragma once

#include <FeatureHelpers/FeatureHelpers.h>

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

template <typename HookContext>
struct HudFeatures {
    HudFeatures(HudFeaturesStates& states, HookContext& hookContext) noexcept
        : states{states}
        , hookContext{hookContext}
    {
    }

    [[nodiscard]] auto bombTimerToggle() const noexcept
    {
        return BombTimerToggle{BombTimerContext{hookContext}};
    }

    [[nodiscard]] auto defusingAlert() const noexcept
    {
        return DefusingAlert{DefusingAlertContext{hookContext, states.defusingAlertState}};
    }

    [[nodiscard]] auto defusingAlertToggle() const noexcept
    {
        return DefusingAlertToggle{DefusingAlertContext{hookContext, states.defusingAlertState}};
    }
    
    [[nodiscard]] auto killfeedPreserver() const noexcept
    {
        return KillfeedPreserver{KillfeedPreserverContext{states.killfeedPreserverState, hookContext}};
    }

    [[nodiscard]] auto killfeedPreserveToggle() const noexcept
    {
        return KillfeedPreserveToggle{KillfeedPreserverContext{states.killfeedPreserverState, hookContext}};
    }

    [[nodiscard]] decltype(auto) postRoundTimerToggle() const noexcept
    {
        return hookContext.template make<PostRoundTimerToggle>();
    }

    HudFeaturesStates& states;
    HookContext& hookContext;
};
