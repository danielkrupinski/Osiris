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

    [[nodiscard]] bool enabled() const
    {
        return hookContext.config().template getVariable<model_glow_vars::GlowTickingBomb>();
    }

    [[nodiscard]] bool shouldApplyGlow(auto&& plantedBomb) const
    {
        return plantedBomb.isTicking().valueOr(true);
    }

    [[nodiscard]] bool& disablingFlag() const
    {
        return state().tickingBombModelGlowDisabling;
    }

    [[nodiscard]] color::HueInteger hue() const
    {
        return hookContext.config().template getVariable<model_glow_vars::TickingBombHue>();
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
