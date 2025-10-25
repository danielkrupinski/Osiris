#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <HookContext/HookContextMacros.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class DroppedBombOutlineGlow {
public:
    explicit DroppedBombOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(outline_glow_vars::GlowDroppedBomb);
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& bomb) const
    {
        return !bomb.hasOwner().valueOr(true);
    }

    [[nodiscard]] color::HueInteger hue() const
    {
        return GET_CONFIG_VAR(outline_glow_vars::DroppedBombHue);
    }

private:
    HookContext& hookContext;
};
