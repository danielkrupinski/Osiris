#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class DroppedBombOutlineGlow {
public:
    explicit DroppedBombOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& bomb) const noexcept
    {
        return enabled() && shouldGlowBomb(bomb);
    }

    [[nodiscard]] Optional<color::Hue> getGlowHue(EntityTypeInfo /* entityTypeInfo */, auto&& /* bomb */) const noexcept
    {
        return outline_glow_params::kDroppedBombHue.toHueFloat();
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return hookContext.config().template getVariable<DroppedBombOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowBomb(auto&& bomb) const noexcept
    {
        return !bomb.hasOwner().valueOr(true);
    }

    HookContext& hookContext;
};
