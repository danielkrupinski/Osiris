#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/TeamNumber.h>
#include "PlayerOutlineGlowColorType.h"

template <typename HookContext>
class PlayerOutlineGlow {
public:
    explicit PlayerOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void applyGlowToPlayer(auto&& playerPawn) const noexcept
    {
        if (!shouldRun() || !shouldGlowPlayer(playerPawn))
            return;

        const auto color = correctAlpha(playerPawn, getColor(playerPawn));
        playerPawn.baseEntity().applyGlow(color);
        playerPawn.baseEntity().forEachChild([color](auto&& entity) { entity.applyGlow(color); });
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return hookContext.config().template getVariable<PlayerOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowPlayer(auto&& playerPawn) const noexcept
    {
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!hookContext.config().template getVariable<PlayerOutlineGlowOnlyEnemies>() || playerPawn.isEnemy().value_or(true));
    }

    [[nodiscard]] cs2::Color correctAlpha(auto&& playerPawn, cs2::Color color) const noexcept
    {
        constexpr auto kNormalAlpha = 102;
        constexpr auto kImmuneAlpha = 40;
        return color.setAlpha(playerPawn.hasImmunity().valueOr(false) ? kImmuneAlpha : kNormalAlpha);
    }

    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        if (hookContext.config().template getVariable<PlayerOutlineGlowColorMode>() == PlayerOutlineGlowColorType::HealthBased)
            return playerPawn.healthColor().value_or(cs2::kColorWhite);

        if (hookContext.config().template getVariable<PlayerOutlineGlowColorMode>() == PlayerOutlineGlowColorType::PlayerOrTeamColor) {
            if (const auto playerColor = playerPawn.playerController().getPlayerColor(); playerColor.has_value())
                return *playerColor;
        }

        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return cs2::kColorTeamTT;
        case TeamNumber::CT: return cs2::kColorTeamCT;
        default: return cs2::kColorWhite;
        }
    }

    HookContext& hookContext;
};
