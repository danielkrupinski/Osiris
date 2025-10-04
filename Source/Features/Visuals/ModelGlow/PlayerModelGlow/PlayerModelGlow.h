#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/TeamNumber.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <Utils/ColorUtils.h>

extern "C" std::uint64_t PlayerPawn_sceneObjectUpdater_asm(cs2::C_CSPlayerPawn* playerPawn, void* unknown, bool unknownBool) noexcept;

template <typename HookContext>
class PlayerModelGlow {
public:
    explicit PlayerModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return hookContext.config().template getVariable<model_glow_vars::GlowPlayers>();
    }

    [[nodiscard]] bool shouldApplyGlow(auto&& playerPawn) const
    {
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!getConfigVariable<model_glow_vars::GlowOnlyEnemies>() || playerPawn.isEnemy().value_or(true));
    }

    [[nodiscard]] bool& disablingFlag() const
    {
        return state().playerModelGlowDisabling;
    }

    [[nodiscard]] auto& originalSceneObjectUpdater() const
    {
        return state().originalPlayerPawnSceneObjectUpdater;
    }

    [[nodiscard]] auto replacementSceneObjectUpdater() const
    {
        return &PlayerPawn_sceneObjectUpdater_asm;
    }

    [[nodiscard]] Optional<color::Hue> hue(auto&& playerPawn) const
    {
        switch (getConfigVariable<model_glow_vars::PlayerGlowColorMode>()) {
        using enum PlayerModelGlowColorType;
        case EnemyAlly: return enemyAllyColorModeHue(playerPawn);
        case HealthBased: return healthBasedColorModeHue(playerPawn);
        case PlayerOrTeamColor:
            if (const auto playerColor = getPlayerColorHue(playerPawn.playerController().playerColorIndex()); playerColor.hasValue())
                return playerColor.value().toHueFloat();
            [[fallthrough]];
        case TeamColor: return teamColorModeHue(playerPawn);
        default: return {};
        }
    }

    [[nodiscard]] color::Saturation saturation(auto&& playerPawn) const
    {
        return playerPawn.hasImmunity().valueOr(false)
            ? model_glow_params::kImmunePlayerColorSaturation
            : model_glow_params::kNormalPlayerColorSaturation;
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    [[nodiscard]] Optional<color::Hue> teamColorModeHue(auto&& playerPawn) const noexcept
    {
        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return static_cast<color::HueInteger>(getConfigVariable<model_glow_vars::TeamTHue>()).toHueFloat();
        case TeamNumber::CT: return static_cast<color::HueInteger>(getConfigVariable<model_glow_vars::TeamCTHue>()).toHueFloat();
        default: return {};
        }
    }

    [[nodiscard]] Optional<color::Hue> enemyAllyColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto isEnemy = playerPawn.isEnemy(); isEnemy.has_value())
            return (*isEnemy ? static_cast<color::HueInteger>(getConfigVariable<model_glow_vars::EnemyHue>()) : static_cast<color::HueInteger>(getConfigVariable<model_glow_vars::AllyHue>())).toHueFloat();
        return {};
    }

    [[nodiscard]] Optional<color::Hue> healthBasedColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue()) {
            const auto fraction = healthFraction(healthValue.value());

            const color::HueInteger lowHealthHue{getConfigVariable<model_glow_vars::LowHealthHue>()};
            const color::HueInteger highHealthHue{getConfigVariable<model_glow_vars::HighHealthHue>()};
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

    [[nodiscard]] Optional<color::HueInteger> getPlayerColorHue(auto playerColorIndex) const noexcept
    {
        if (!playerColorIndex.hasValue())
            return {};

        switch (playerColorIndex.value()) {
        using enum cs2::PlayerColorIndex;
        case Blue: return getConfigVariable<model_glow_vars::PlayerBlueHue>();
        case Green: return getConfigVariable<model_glow_vars::PlayerGreenHue>();
        case Yellow: return getConfigVariable<model_glow_vars::PlayerYellowHue>();
        case Orange: return getConfigVariable<model_glow_vars::PlayerOrangeHue>();
        case Purple: return getConfigVariable<model_glow_vars::PlayerPurpleHue>();
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
