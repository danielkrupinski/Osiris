#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <GameClient/Entities/TeamNumber.h>
#include <Utils/ColorUtils.h>
#include "PlayerOutlineGlowColorType.h"

template <typename HookContext>
class PlayerOutlineGlow {
public:
    explicit PlayerOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& playerPawn) const noexcept
    {
        return enabled() && shouldGlowPlayer(playerPawn);
    }

    [[nodiscard]] Optional<color::Hue> getGlowHue(EntityTypeInfo /* entityTypeInfo */, auto&& playerPawn) const noexcept
    {
        switch (getConfigVariable<PlayerOutlineGlowColorMode>()) {
        using enum PlayerOutlineGlowColorType;
        case EnemyAlly:
            if (const auto hue = enemyAllyColorModeHue(playerPawn))
                return hue->toHueFloat();
            return {};
        case HealthBased:
            return healthBasedColorModeHue(playerPawn);
        case PlayerOrTeamColor:
            if (const auto playerColor = getPlayerColorHue(playerPawn.playerController().playerColorIndex()))
                return playerColor->toHueFloat();
            [[fallthrough]];
        case TeamColor:
            if (const auto teamHue = teamColorModeHue(playerPawn))
                return teamHue->toHueFloat();
        default:
            return {};
        }
    }

    [[nodiscard]] std::uint8_t getGlowColorAlpha(auto&& playerPawn) const noexcept
    {
        using namespace outline_glow_params;
        return playerPawn.hasImmunity().valueOr(false) ? kImmunePlayerGlowAlpha : kGlowAlpha;
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return getConfigVariable<PlayerOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowPlayer(auto&& playerPawn) const noexcept
    {
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!getConfigVariable<PlayerOutlineGlowOnlyEnemies>() || playerPawn.isEnemy().value_or(true));
    }

    [[nodiscard]] std::optional<color::HueInteger> teamColorModeHue(auto&& playerPawn) const noexcept
    {
        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return getConfigVariable<PlayerOutlineGlowTeamTHue>();
        case TeamNumber::CT: return getConfigVariable<PlayerOutlineGlowTeamCTHue>();
        default: return {};
        }
    }

    [[nodiscard]] std::optional<color::HueInteger> enemyAllyColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto isEnemy = playerPawn.isEnemy(); isEnemy.has_value())
            return *isEnemy ? getConfigVariable<PlayerOutlineGlowEnemyHue>() : getConfigVariable<PlayerOutlineGlowAllyHue>();
        return {};
    }

    [[nodiscard]] Optional<color::Hue> healthBasedColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue()) {
            const auto fraction = healthFraction(healthValue.value());

            const color::HueInteger lowHealthHue{getConfigVariable<PlayerOutlineGlowLowHealthHue>()};
            const color::HueInteger highHealthHue{getConfigVariable<PlayerOutlineGlowHighHealthHue>()};
            if (lowHealthHue < highHealthHue)
                return color::Hue{(lowHealthHue + (highHealthHue - lowHealthHue) * fraction) / 360.0f};
            else
                return color::Hue{(lowHealthHue - (lowHealthHue - highHealthHue) * fraction) / 360.0f};
        }
        return {};
    }

    [[nodiscard]] static constexpr float healthFraction(int playerHealth) noexcept
    {
        constexpr auto kMinHealth = 1;
        constexpr auto kMaxHealth = 100;

        if (playerHealth <= kMinHealth)
            return 0.0f;
        if (playerHealth >= kMaxHealth)
            return 1.0f;
        return static_cast<float>(playerHealth - kMinHealth) / (kMaxHealth - kMinHealth); 
    }

    [[nodiscard]] std::optional<color::HueInteger> getPlayerColorHue(auto playerColorIndex) const noexcept
    {
        if (!playerColorIndex.hasValue())
            return {};

        switch (playerColorIndex.value()) {
        case 0: return getConfigVariable<PlayerOutlineGlowPlayerBlueHue>();
        case 1: return getConfigVariable<PlayerOutlineGlowPlayerGreenHue>();
        case 2: return getConfigVariable<PlayerOutlineGlowPlayerYellowHue>();
        case 3: return getConfigVariable<PlayerOutlineGlowPlayerOrangeHue>();
        case 4: return getConfigVariable<PlayerOutlineGlowPlayerPurpleHue>();
        default: return {};
        }
    }

    template <typename ConfigVariable>
    [[nodiscard]] decltype(auto) getConfigVariable() const noexcept
    {
        return hookContext.config().template getVariable<ConfigVariable>();
    }

    HookContext& hookContext;
};
