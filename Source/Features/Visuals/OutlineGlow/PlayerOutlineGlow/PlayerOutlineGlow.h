#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <GameClient/Entities/TeamNumber.h>
#include <HookContext/HookContextMacros.h>
#include <Utils/ColorUtils.h>
#include "PlayerOutlineGlowColorType.h"

template <typename HookContext>
class PlayerOutlineGlow {
public:
    explicit PlayerOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(outline_glow_vars::GlowPlayers);
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& playerPawn) const noexcept
    {
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!GET_CONFIG_VAR(outline_glow_vars::GlowOnlyEnemies) || playerPawn.isEnemy().value_or(true));
    }

    [[nodiscard]] Optional<color::Hue> hue(EntityTypeInfo /* entityTypeInfo */, auto&& playerPawn) const noexcept
    {
        switch (GET_CONFIG_VAR(outline_glow_vars::PlayerGlowColorMode)) {
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
    [[nodiscard]] std::optional<color::HueInteger> teamColorModeHue(auto&& playerPawn) const noexcept
    {
        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return GET_CONFIG_VAR(outline_glow_vars::TeamTHue);
        case TeamNumber::CT: return GET_CONFIG_VAR(outline_glow_vars::TeamCTHue);
        default: return {};
        }
    }

    [[nodiscard]] std::optional<color::HueInteger> enemyAllyColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto isEnemy = playerPawn.isEnemy(); isEnemy.has_value())
            return *isEnemy ? GET_CONFIG_VAR(outline_glow_vars::EnemyHue) : GET_CONFIG_VAR(outline_glow_vars::AllyHue);
        return {};
    }

    [[nodiscard]] Optional<color::Hue> healthBasedColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue()) {
            const auto fraction = healthFraction(healthValue.value());

            const color::HueInteger lowHealthHue{GET_CONFIG_VAR(outline_glow_vars::LowHealthHue)};
            const color::HueInteger highHealthHue{GET_CONFIG_VAR(outline_glow_vars::HighHealthHue)};
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
        using enum cs2::PlayerColorIndex;
        case Blue: return GET_CONFIG_VAR(outline_glow_vars::PlayerBlueHue);
        case Green: return GET_CONFIG_VAR(outline_glow_vars::PlayerGreenHue);
        case Yellow: return GET_CONFIG_VAR(outline_glow_vars::PlayerYellowHue);
        case Orange: return GET_CONFIG_VAR(outline_glow_vars::PlayerOrangeHue);
        case Purple: return GET_CONFIG_VAR(outline_glow_vars::PlayerPurpleHue);
        default: return {};
        }
    }

    HookContext& hookContext;
};
