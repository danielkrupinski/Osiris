#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowState.h>

#include "DefuseKitModelGlowParams.h"

template <typename HookContext>
class DefuseKitModelGlow {
public:
    explicit DefuseKitModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onEntityListTraversed() const noexcept
    {
        if (state().defuseKitModelGlow == ModelGlowState::State::Disabling)
            state().defuseKitModelGlow = ModelGlowState::State::Disabled;
    }

    void updateModelGlow(auto&& defuseKit) const noexcept
    {
        if (isDisabled())
            return;

        using namespace defuse_kit_model_glow_params;
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
        return state().defuseKitModelGlow == ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return state().defuseKitModelGlow == ModelGlowState::State::Enabled;
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
