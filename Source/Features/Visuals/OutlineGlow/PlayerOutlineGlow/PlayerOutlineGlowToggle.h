#pragma once

#include <utility>
#include <Features/Common/FeatureToggle.h>
#include "PlayerOutlineGlowColorType.h"

template <typename HookContext>
struct PlayerOutlineGlowToggle : FeatureToggle<PlayerOutlineGlowToggle<HookContext>> {
    explicit PlayerOutlineGlowToggle(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void update(char option) noexcept
    {
        switch (option) {
        case '0': this->enable(); state().showOnlyEnemies = true; break;
        case '1': this->enable(); state().showOnlyEnemies = false; break;
        case '2': this->disable(); break;
        }
    }

    void updateColor(char option) noexcept
    {
        switch (option) {
        case '0': state().playerGlowColorType = PlayerOutlineGlowColorType::PlayerOrTeamColor; break;
        case '1': state().playerGlowColorType = PlayerOutlineGlowColorType::TeamColor; break;
        case '2': state().playerGlowColorType = PlayerOutlineGlowColorType::HealthBased; break;
        }
    }

    [[nodiscard]] auto& enabledVariable(typename PlayerOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return state().enabledForPlayers;
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    HookContext& hookContext;
};
