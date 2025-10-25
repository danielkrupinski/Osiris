#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <HookContext/HookContextMacros.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class DefuseKitOutlineGlow {
public:
    explicit DefuseKitOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(outline_glow_vars::GlowDefuseKits);
    }

    [[nodiscard]] color::HueInteger hue() const
    {
        return GET_CONFIG_VAR(outline_glow_vars::DefuseKitHue);
    }

    [[nodiscard]] int getGlowRange() const noexcept
    {
        return outline_glow_params::kDefuseKitGlowRange;
    }

private:
    HookContext& hookContext;
};
