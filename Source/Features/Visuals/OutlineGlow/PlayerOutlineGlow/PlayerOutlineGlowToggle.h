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
        case '0': hookContext.config().template setVariable<outline_glow_vars::GlowPlayers>(true); hookContext.config().template setVariable<outline_glow_vars::GlowOnlyEnemies>(true); break;
        case '1': hookContext.config().template setVariable<outline_glow_vars::GlowPlayers>(true); hookContext.config().template setVariable<outline_glow_vars::GlowOnlyEnemies>(false); break;
        case '2': hookContext.config().template setVariable<outline_glow_vars::GlowPlayers>(false); break;
        }
    }

    void updateColor(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.config().template setVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::PlayerOrTeamColor); break;
        case '1': hookContext.config().template setVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::TeamColor); break;
        case '2': hookContext.config().template setVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::HealthBased); break;
        case '3': hookContext.config().template setVariable<outline_glow_vars::PlayerGlowColorMode>(PlayerOutlineGlowColorType::EnemyAlly); break;
        }
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    HookContext& hookContext;
};
