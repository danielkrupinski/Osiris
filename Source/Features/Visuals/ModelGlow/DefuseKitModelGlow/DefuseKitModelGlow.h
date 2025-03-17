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

        using namespace model_glow_params;
        if (isEnabled())
            defuseKit.baseEntity().applySpawnProtectionEffectRecursively(kDefuseKitColor);
        else
            defuseKit.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    void onUnload(auto&& defuseKit) const noexcept
    {
        if (!isDisabled())
            defuseKit.baseEntity().removeSpawnProtectionEffectRecursively();
    }

private:
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
