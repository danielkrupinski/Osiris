#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>

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

        if (shouldGlowPlantedBombModel(plantedBomb))
            plantedBomb.baseEntity().applySpawnProtectionEffectRecursively(model_glow_params::kTickingBombColor);
        else
            plantedBomb.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    void onUnload(auto&& plantedBomb) const noexcept
    {
        if (!isDisabled())
            plantedBomb.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    void enable() const noexcept
    {
        state().tickingBombModelGlow = ModelGlowState::State::Enabled;
    }

    void disable() const noexcept
    {
        if (state().tickingBombModelGlow == ModelGlowState::State::Enabled)
            state().tickingBombModelGlow = ModelGlowState::State::Disabling;
    }

private:
    [[nodiscard]] bool isDisabled() const noexcept
    {
        return state().tickingBombModelGlow == ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return state().masterSwitch == ModelGlowState::State::Enabled && state().tickingBombModelGlow == ModelGlowState::State::Enabled;
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
