#pragma once

#include <FeatureHelpers/FeatureToggle.h>

template <typename Context>
struct PlayerOutlineGlowToggle : FeatureToggleOnOff<PlayerOutlineGlowToggle<Context>> {
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

    [[nodiscard]] auto& enabledVariable(typename PlayerOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

    void onEnable(typename PlayerOutlineGlowToggle::ToggleMethod) noexcept
    {
        context.viewRenderHook().incrementReferenceCount();
    }

    void onDisable(typename PlayerOutlineGlowToggle::ToggleMethod) noexcept
    {
        context.viewRenderHook().decrementReferenceCount();
        context.clearGlowSceneObjects();
    }

private:
    Context context;
};

template <typename HookContext>
class PlayerOutlineGlowContext;

template <typename HookContext>
PlayerOutlineGlowToggle(HookContext&) -> PlayerOutlineGlowToggle<PlayerOutlineGlowContext<HookContext>>;
