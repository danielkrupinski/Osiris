#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowState.h>

#include "TickingBombModelGlowParams.h"

template <typename HookContext>
class TickingBombModelGlow {
public:
    explicit TickingBombModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onEntityListTraversed() const noexcept
    {
        if (state().tickingBombModelGlow == ModelGlowState::State::Disabling)
            state().tickingBombModelGlow = ModelGlowState::State::Disabled;
    }

    void applyModelGlow(auto&& plantedBomb) const noexcept
    {
        if (isDisabled())
            return;

        using namespace ticking_bomb_model_glow_params;
        if (shouldGlowPlantedBombModel(plantedBomb))
            plantedBomb.baseEntity().applySpawnProtectionEffectRecursively(kTickingBombColor);
        else
            plantedBomb.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    void onUnload(auto&& plantedBomb) const noexcept
    {
        if (!isDisabled())
            plantedBomb.baseEntity().removeSpawnProtectionEffectRecursively();
    }

private:
    [[nodiscard]] bool isDisabled() const noexcept
    {
        return state().tickingBombModelGlow == ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return state().tickingBombModelGlow == ModelGlowState::State::Enabled;
    }

    [[nodiscard]] bool shouldGlowPlantedBombModel(auto&& plantedBomb) const noexcept
    {
        return isEnabled() && plantedBomb.isTicking().valueOr(true);
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
