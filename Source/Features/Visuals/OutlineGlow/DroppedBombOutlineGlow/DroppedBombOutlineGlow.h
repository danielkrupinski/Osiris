#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>

template <typename HookContext>
class DroppedBombOutlineGlow {
public:
    explicit DroppedBombOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void applyGlowToBomb(auto&& bomb) const noexcept
    {
        if (enabled() && shouldGlowBomb(bomb))
            bomb.applyGlowRecursively(outline_glow_params::kDroppedBombColor);
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
