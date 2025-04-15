#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <GameClient/Entities/EntityClassifier.h>

template <typename HookContext>
class DefuseKitOutlineGlow {
public:
    explicit DefuseKitOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& /* defuseKit */) const noexcept
    {
        return hookContext.config().template getVariable<DefuseKitOutlineGlowEnabled>();
    }

    [[nodiscard]] cs2::Color getGlowColor(EntityTypeInfo /* entityTypeInfo */, auto&& /* defuseKit */) const noexcept
    {
        return outline_glow_params::kDefuseKitColor;
    }

    [[nodiscard]] int getGlowRange() const noexcept
    {
        return outline_glow_params::kDefuseKitGlowRange;
    }

private:
    HookContext& hookContext;
};
