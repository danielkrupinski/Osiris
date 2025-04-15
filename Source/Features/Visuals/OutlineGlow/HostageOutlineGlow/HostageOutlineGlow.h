#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <GameClient/Entities/EntityClassifier.h>

template <typename HookContext>
class HostageOutlineGlow {
public:
    explicit HostageOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& /* hostage */) const noexcept
    {
        return hookContext.config().template getVariable<HostageOutlineGlowEnabled>();
    }

    [[nodiscard]] cs2::Color getGlowColor(EntityTypeInfo /* entityTypeInfo */, auto&& /* hostage */) const noexcept
    {
        return outline_glow_params::kHostageColor;
    }

private:
    HookContext& hookContext;
};
