#pragma once

#include <utility>

#include "DroppedBombOutlineGlowContext.h"
#include "DroppedBombOutlineGlowParams.h"

template <typename Context>
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

        using namespace dropped_bomb_outline_glow_params;
        bomb.applyGlowRecursively(kColor);
    }

private:
    Context context;
};

template <typename HookContext>
DroppedBombOutlineGlow(HookContext&) -> DroppedBombOutlineGlow<DroppedBombOutlineGlowContext<HookContext>>;
