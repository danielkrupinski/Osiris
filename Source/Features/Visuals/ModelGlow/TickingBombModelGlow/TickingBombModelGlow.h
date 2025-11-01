#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
class TickingBombModelGlow {
public:
    explicit TickingBombModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(model_glow_vars::GlowTickingBomb);
    }

    [[nodiscard]] bool shouldApplyGlow(auto&& plantedBomb) const
    {
        return plantedBomb.isTicking().valueOr(true);
    }

    [[nodiscard]] auto deactivationFlag() const noexcept
    {
        return ModelGlowDeactivationFlags::TickingBombModelGlowDeactivating;
    }

    [[nodiscard]] color::HueInteger hue() const
    {
        return GET_CONFIG_VAR(model_glow_vars::TickingBombHue);
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
