#pragma once

#include <FeatureHelpers/FeatureToggle.h>
#include "PlayerOutlineGlowColorType.h"

template <typename HookContext>
class PlayerOutlineGlowContext;

template <typename HookContext, typename Context = PlayerOutlineGlowContext<HookContext>>
struct PlayerOutlineGlowToggle : FeatureToggle<PlayerOutlineGlowToggle<HookContext, Context>> {
    template <typename... Args>
    PlayerOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(char option) noexcept
    {
        switch (option) {
        case '0': this->enable(); context.state().showOnlyEnemies = true; break;
        case '1': this->enable(); context.state().showOnlyEnemies = false; break;
        case '2': this->disable(); break;
        }
    }

    void updateColor(char option) noexcept
    {
        switch (option) {
        case '0': context.state().colorType = PlayerOutlineGlowColorType::PlayerOrTeamColor; break;
        case '1': context.state().colorType = PlayerOutlineGlowColorType::TeamColor; break;
        case '2': context.state().colorType = PlayerOutlineGlowColorType::HealthBased; break;
        }
    }

    [[nodiscard]] auto& enabledVariable(typename PlayerOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

private:
    Context context;
};
