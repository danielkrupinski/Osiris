#pragma once

#include <utility>

#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/TeamNumber.h>
#include "PlayerOutlineGlowContext.h"
#include "PlayerOutlineGlowColorType.h"

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

        const auto color = correctAlpha(playerPawn, getColor(playerPawn));
        playerPawn.baseEntity().applyGlow(color);
        playerPawn.baseEntity().forEachChild([color](auto&& entity) { entity.applyGlow(color); });
    }

private:
    [[nodiscard]] cs2::Color correctAlpha(auto&& playerPawn, cs2::Color color) const noexcept
    {
        constexpr auto kNormalAlpha = 102;
        constexpr auto kImmuneAlpha = 40;
        return color.setAlpha(playerPawn.hasImmunity().valueOr(false) ? kImmuneAlpha : kNormalAlpha);
    }

    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        if (context.state().colorType == PlayerOutlineGlowColorType::HealthBased)
            return playerPawn.healthColor().value_or(cs2::kColorWhite);

        if (context.state().colorType == PlayerOutlineGlowColorType::PlayerOrTeamColor) {
            if (const auto playerColor = playerPawn.playerController().getPlayerColor(); playerColor.has_value())
                return *playerColor;
        }

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
