#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
class DefuseKitModelGlow {
public:
    explicit DefuseKitModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(model_glow_vars::GlowDefuseKits);    
    }

    [[nodiscard]] bool& disablingFlag() const
    {
        return state().defuseKitModelGlowDisabling;
    }

    [[nodiscard]] color::HueInteger hue() const
    {
        return GET_CONFIG_VAR(model_glow_vars::DefuseKitHue);
    }

private:
    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
