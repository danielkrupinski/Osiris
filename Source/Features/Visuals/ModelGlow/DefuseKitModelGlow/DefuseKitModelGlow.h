#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>

template <typename HookContext>
class DefuseKitModelGlow {
public:
    explicit DefuseKitModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onEntityListTraversed() const noexcept
    {
        state().defuseKitModelGlowDisabling = false;
    }

    void updateModelGlow(auto&& defuseKit) const noexcept
    {
        if (isDisabled())
            return;

        if (isEnabled())
            defuseKit.baseEntity().applySpawnProtectionEffectRecursively(getColor());
        else
            defuseKit.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    void onUnload(auto&& defuseKit) const noexcept
    {
        if (!isDisabled())
            defuseKit.baseEntity().removeSpawnProtectionEffectRecursively();
    }

private:
    [[nodiscard]] cs2::Color getColor() const noexcept
    {
        return color::HSBtoRGB(hookContext.config().template getVariable<ModelGlowDefuseKitHue>(), color::Saturation{1.0f}, color::Brightness{1.0f});
    }

    [[nodiscard]] bool isDisabled() const noexcept
    {
        return !isEnabled() && !state().defuseKitModelGlowDisabling;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return hookContext.config().template getVariable<ModelGlowEnabled>() && hookContext.config().template getVariable<DefuseKitModelGlowEnabled>();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
