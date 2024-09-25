#pragma once

#include <utility>

#include "TickingBombOutlineGlowContext.h"
#include "TickingBombOutlineGlowParams.h"

template <typename Context>
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

        using namespace ticking_bomb_outline_glow_params;
        plantedBomb.baseEntity().applyGlowRecursively(kColor);
    }

private:
    Context context;
};

template <typename HookContext>
TickingBombOutlineGlow(HookContext&) -> TickingBombOutlineGlow<TickingBombOutlineGlowContext<HookContext>>;
