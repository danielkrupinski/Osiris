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
        state().tickingBombModelGlowDisabling = false;
    }

    void applyModelGlow(auto&& plantedBomb) const noexcept
    {
        if (isDisabled())
            return;

        if (shouldGlowPlantedBombModel(plantedBomb))
            plantedBomb.baseEntity().applySpawnProtectionEffectRecursively(getColor());
        else
            plantedBomb.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    void onUnload(auto&& plantedBomb) const noexcept
    {
        if (!isDisabled())
            plantedBomb.baseEntity().removeSpawnProtectionEffectRecursively();
    }

private:
    [[nodiscard]] cs2::Color getColor() const noexcept
    {
        return color::HSBtoRGB(hookContext.config().template getVariable<ModelGlowTickingBombHue>(), color::Saturation{1.0f}, color::Brightness{1.0f});
    }

    [[nodiscard]] bool isDisabled() const noexcept
    {
        return !hookContext.config().template getVariable<TickingBombModelGlowEnabled>() && !state().tickingBombModelGlowDisabling;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return hookContext.config().template getVariable<ModelGlowEnabled>() && hookContext.config().template getVariable<TickingBombModelGlowEnabled>();
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
