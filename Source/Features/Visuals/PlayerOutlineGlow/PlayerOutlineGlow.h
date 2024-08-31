#pragma once

#include <utility>

#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/TeamNumber.h>
#include "PlayerOutlineGlowContext.h"

template <typename Context>
class PlayerOutlineGlow {
public:
    template <typename... Args>
    PlayerOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToPlayer(auto&& playerPawn) const noexcept
    {
        auto&& condition = context.condition();
        if (!condition.shouldRun() || !condition.shouldGlowPlayer(playerPawn))
            return;

        applyGlow(playerPawn.baseEntity(), getColor(playerPawn));
    }

private:
    void applyGlow(auto&& baseEntity, cs2::Color color) const noexcept
    {
        context.getGlowSceneObjectFor(baseEntity).apply(baseEntity, color.setAlpha(102));
    }

    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return cs2::kColorTeamTT;
        case TeamNumber::CT: return cs2::kColorTeamCT;
        default: return cs2::kColorWhite;
        }
    }

    Context context;
};

template <typename HookContext>
PlayerOutlineGlow(HookContext&) -> PlayerOutlineGlow<PlayerOutlineGlowContext<HookContext>>;
