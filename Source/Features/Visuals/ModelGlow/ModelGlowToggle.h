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
            if (state().masterSwitch != ModelGlowState::State::Enabled) {
                if (state().masterSwitch == ModelGlowState::State::Disabled)
                    hookContext.hooks().viewRenderHook.incrementReferenceCount();
                state().masterSwitch = ModelGlowState::State::Enabled;
            }
            break;
        case '1':
            if (state().masterSwitch == ModelGlowState::State::Enabled)
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

    void updatePlayerModelGlowColor(char option) noexcept
    {
        switch (option) {
        case '0': state().playerModelGlowColorType = PlayerModelGlowColorType::PlayerOrTeamColor; break;
        case '1': state().playerModelGlowColorType = PlayerModelGlowColorType::TeamColor; break;
        }
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
