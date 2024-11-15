#pragma once

#include <FeatureHelpers/FeatureToggle.h>

template <typename HookContext>
class ModelGlowToggle {
public:
    ModelGlowToggle(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateMasterSwitch(char option) noexcept
    {
        switch (option) {
        case '0':
            state().masterSwitch = ModelGlowState::State::Enabled;
            hookContext.hooks().viewRenderHook.incrementReferenceCount();
            break;
        case '1':
            state().masterSwitch = ModelGlowState::State::Disabling;
            break;
        }
    }

    void updatePlayerModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': state().playerModelGlow = ModelGlowState::State::Enabled; state().showOnlyEnemies = true; break;
        case '1': state().playerModelGlow = ModelGlowState::State::Enabled; state().showOnlyEnemies = false; break;
        case '2': state().playerModelGlow = ModelGlowState::State::Disabling; break;
        }
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
