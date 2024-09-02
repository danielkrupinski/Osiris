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

        const auto glowColor = getColor(playerPawn).setAlpha(102);
        glowPlayer(playerPawn, glowColor);
        glowPlayerWeapons(playerPawn, glowColor);
    }

private:
    void glowPlayer(auto&& playerPawn, cs2::Color glowColor) const noexcept
    {
        applyGlow(playerPawn.baseEntity(), glowColor);
    }

    void glowPlayerWeapons(auto&& playerPawn, cs2::Color glowColor) const noexcept
    {
        playerPawn.weapons().forEach([this, glowColor](auto&& weapon) { this->applyGlow(weapon, glowColor); });
    }

    void applyGlow(auto&& baseEntity, cs2::Color color) const noexcept
    {
        context.getGlowSceneObjectFor(baseEntity).apply(baseEntity, color);
    }

    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
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
