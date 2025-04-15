#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <GameClient/Entities/EntityClassifier.h>

template <typename HookContext>
class TickingBombOutlineGlow {
public:
    TickingBombOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& plantedBomb) const noexcept
    {
        return enabled() && shouldGlowPlantedBomb(plantedBomb);
    }

    [[nodiscard]] cs2::Color getGlowColor(EntityTypeInfo /* entityTypeInfo */, auto&& /* plantedBomb */) const noexcept
    {
        return outline_glow_params::kTickingBombColor;
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return hookContext.config().template getVariable<TickingBombOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowPlantedBomb(auto&& plantedBomb) const noexcept
    {
        return plantedBomb.isTicking().valueOr(true);
    }

    HookContext& hookContext;
};
