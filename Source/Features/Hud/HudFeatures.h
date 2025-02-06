#pragma once

#include "BombTimer/BombTimerContext.h"
#include "BombTimer/BombTimerToggle.h"
#include "DefusingAlert/DefusingAlertContext.h"
#include "DefusingAlert/DefusingAlertToggle.h"
#include "KillfeedPreserver/KillfeedPreserverContext.h"
#include "KillfeedPreserver/KillfeedPreserveToggle.h"
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

    [[nodiscard]] auto defusingAlertToggle() const noexcept
    {
        return DefusingAlertToggle{DefusingAlertContext{hookContext}};
    }

    [[nodiscard]] auto killfeedPreserveToggle() const noexcept
    {
        return KillfeedPreserveToggle{KillfeedPreserverContext{hookContext}};
    }

    [[nodiscard]] decltype(auto) postRoundTimerToggle() const noexcept
    {
        return hookContext.template make<PostRoundTimerToggle>();
    }

    HudFeaturesStates& states;
    HookContext& hookContext;
};
