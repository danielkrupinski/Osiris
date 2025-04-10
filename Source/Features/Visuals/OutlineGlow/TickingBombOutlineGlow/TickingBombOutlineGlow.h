#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>

template <typename HookContext>
class TickingBombOutlineGlow {
public:
    TickingBombOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void applyGlowToPlantedBomb(auto&& plantedBomb) const noexcept
    {
        if (shouldRun() && shouldGlowPlantedBomb(plantedBomb))
            plantedBomb.baseEntity().applyGlowRecursively(outline_glow_params::kTickingBombColor);
    }

private:
    [[nodiscard]] bool shouldRun() const noexcept
    {
        return hookContext.config().template getVariable<TickingBombOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowPlantedBomb(auto&& plantedBomb) const noexcept
    {
        return plantedBomb.isTicking().valueOr(true);
    }

    HookContext& hookContext;
};
