#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class HostageOutlineGlow {
public:
    explicit HostageOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& /* hostage */) const noexcept
    {
        return hookContext.config().template getVariable<outline_glow_vars::GlowHostages>();
    }

    [[nodiscard]] Optional<color::Hue> getGlowHue(EntityTypeInfo /* entityTypeInfo */, auto&& /* hostage */) const noexcept
    {
        return outline_glow_params::kHostageHue.toHueFloat();
    }

private:
    HookContext& hookContext;
};
