#pragma once

#include <utility>
#include <Features/Common/FeatureToggle.h>
#include "PlayerOutlineGlowColorType.h"

template <typename HookContext>
struct PlayerOutlineGlowToggle {
    explicit PlayerOutlineGlowToggle(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.config().template setVariable<PlayerOutlineGlowEnabled>(true); hookContext.config().template setVariable<PlayerOutlineGlowOnlyEnemies>(true); break;
        case '1': hookContext.config().template setVariable<PlayerOutlineGlowEnabled>(true); hookContext.config().template setVariable<PlayerOutlineGlowOnlyEnemies>(false); break;
        case '2': hookContext.config().template setVariable<PlayerOutlineGlowEnabled>(false); break;
        }
    }

    void updateColor(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.config().template setVariable<PlayerOutlineGlowColorMode>(PlayerOutlineGlowColorType::PlayerOrTeamColor); break;
        case '1': hookContext.config().template setVariable<PlayerOutlineGlowColorMode>(PlayerOutlineGlowColorType::TeamColor); break;
        case '2': hookContext.config().template setVariable<PlayerOutlineGlowColorMode>(PlayerOutlineGlowColorType::HealthBased); break;
        }
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    HookContext& hookContext;
};
