#pragma once

#include <utility>

#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include "TickingBombOutlineGlowContext.h"

template <typename HookContext, typename Context = TickingBombOutlineGlowContext<HookContext>>
class TickingBombOutlineGlow {
public:
    template <typename... Args>
    TickingBombOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToPlantedBomb(auto&& plantedBomb) const noexcept
    {
        auto&& condition = context.condition();
        if (!condition.shouldRun() || !condition.shouldGlowPlantedBomb(plantedBomb))
            return;

        plantedBomb.baseEntity().applyGlowRecursively(outline_glow_params::kTickingBombColor);
    }

private:
    Context context;
};
