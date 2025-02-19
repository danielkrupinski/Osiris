#pragma once

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
        if (state().defuseKitModelGlow == ModelGlowState::State::Disabling)
            state().defuseKitModelGlow = ModelGlowState::State::Disabled;
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

    void enable() const noexcept
    {
        state().defuseKitModelGlow = ModelGlowState::State::Enabled;
    }

    void disable() const noexcept
    {
        if (state().defuseKitModelGlow == ModelGlowState::State::Enabled)
            state().defuseKitModelGlow = ModelGlowState::State::Disabling;
    }

private:
    [[nodiscard]] bool isDisabled() const noexcept
    {
        return state().defuseKitModelGlow == ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return state().masterSwitch == ModelGlowState::State::Enabled && state().defuseKitModelGlow == ModelGlowState::State::Enabled;
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
