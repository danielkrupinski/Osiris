#pragma once

#include <utility>

#include "DroppedBombOutlineGlowContext.h"
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>

template <typename HookContext, typename Context = DroppedBombOutlineGlowContext<HookContext>>
class DroppedBombOutlineGlow {
public:
    template <typename... Args>
    DroppedBombOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToBomb(auto&& bomb) const noexcept
    {
        auto&& condition = context.condition();
        if (!condition.shouldRun() || !condition.shouldGlowBomb(bomb))
            return;

        bomb.applyGlowRecursively(outline_glow_params::kDroppedBombColor);
    }

private:
    Context context;
};
