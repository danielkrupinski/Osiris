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

    void onEntityListTraversed() const noexcept
    {
        state().playerModelGlowDisabling = false;
    }
 
    void updateSceneObjectUpdaterHook(auto&& playerPawn) const noexcept
    {
        if (!shouldUpdateSceneObjectUpdaterHook())
            return;

        if (shouldGlowPlayerModel(playerPawn))
            hookPlayerSceneObjectUpdater(playerPawn);
        else
            unhookPlayerSceneObjectUpdater(playerPawn);
    }

    void applyModelGlow(auto&& playerPawn) const noexcept
    {
        if (shouldRun() && shouldGlowPlayerModel(playerPawn))
            playerPawn.baseEntity().applySpawnProtectionEffectRecursively(getColor(playerPawn));
    }

    void onUnload(auto&& playerPawn) const noexcept
    {
        if (getConfigVariable<model_glow_vars::GlowPlayers>() || state().playerModelGlowDisabling)
            unhookPlayerSceneObjectUpdater(playerPawn);
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return getConfigVariable<model_glow_vars::GlowPlayers>() || state().playerModelGlowDisabling;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return getConfigVariable<model_glow_vars::GlowPlayers>();
    }

    void hookPlayerSceneObjectUpdater(auto&& playerPawn) const noexcept
    {
        if (!hasSceneObjectUpdaterHooked(playerPawn)) {
            storeOriginalSceneObjectUpdater(playerPawn);
            hookSceneObjectUpdater(playerPawn);
        }
    }

    void unhookPlayerSceneObjectUpdater(auto&& playerPawn) const noexcept
    {
        if (hasSceneObjectUpdaterHooked(playerPawn))
            playerPawn.setSceneObjectUpdater(state().originalPlayerPawnSceneObjectUpdater);
    }

    void storeOriginalSceneObjectUpdater(auto&& playerPawn) const noexcept
    {
        if (state().originalPlayerPawnSceneObjectUpdater == nullptr)
            state().originalPlayerPawnSceneObjectUpdater = playerPawn.getSceneObjectUpdater();
    }

    [[nodiscard]] bool hasSceneObjectUpdaterHooked(auto&& playerPawn) const noexcept
    {
        return playerPawn.getSceneObjectUpdater() == &PlayerPawn_sceneObjectUpdater_asm;
    }

    void hookSceneObjectUpdater(auto&& playerPawn) const noexcept
    {
        playerPawn.setSceneObjectUpdater(&PlayerPawn_sceneObjectUpdater_asm);
    }

    [[nodiscard]] bool shouldGlowPlayerModel(auto&& playerPawn) const noexcept
    {
        return getConfigVariable<model_glow_vars::Enabled>()
            && playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!getConfigVariable<model_glow_vars::GlowOnlyEnemies>() || playerPawn.isEnemy().value_or(true));
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    [[nodiscard]] color::Saturation getColorSaturation(auto&& playerPawn) const noexcept
    {
        return playerPawn.hasImmunity().valueOr(false) ? model_glow_params::kImmunePlayerColorSaturation : model_glow_params::kNormalPlayerColorSaturation;
    }

    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        if (const auto colorHue = getColorHue(playerPawn))
            return color::HSBtoRGB(*colorHue, getColorSaturation(playerPawn), color::Brightness{1.0f});
        return cs2::kColorWhite;
    }

    [[nodiscard]] std::optional<color::Hue> getColorHue(auto&& playerPawn) const noexcept
    {
        switch (getConfigVariable<model_glow_vars::PlayerGlowColorMode>()) {
        using enum PlayerModelGlowColorType;
        case EnemyAlly:
            return enemyAllyColorModeHue(playerPawn);
        case HealthBased:
            return healthBasedColorModeHue(playerPawn);
        case PlayerOrTeamColor:
            if (const auto playerColor = getPlayerColorHue(playerPawn.playerController().playerColorIndex()))
                return playerColor->toHueFloat();
            [[fallthrough]];
        case TeamColor:
            return teamColorModeHue(playerPawn);
        default:
            return {};
        }
    }

    [[nodiscard]] std::optional<color::Hue> teamColorModeHue(auto&& playerPawn) const noexcept
    {
        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return static_cast<color::HueInteger>(getConfigVariable<model_glow_vars::TeamTHue>()).toHueFloat();
        case TeamNumber::CT: return static_cast<color::HueInteger>(getConfigVariable<model_glow_vars::TeamCTHue>()).toHueFloat();
        default: return {};
        }
    }

    [[nodiscard]] std::optional<color::Hue> enemyAllyColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto isEnemy = playerPawn.isEnemy(); isEnemy.has_value())
            return (*isEnemy ? static_cast<color::HueInteger>(getConfigVariable<model_glow_vars::EnemyHue>()) : static_cast<color::HueInteger>(getConfigVariable<model_glow_vars::AllyHue>())).toHueFloat();
        return {};
    }

    [[nodiscard]] std::optional<color::Hue> healthBasedColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue()) {
            const auto fraction = std::clamp(healthValue.value(), 0, 100) / 100.0f;

            const color::HueInteger lowHealthHue{getConfigVariable<model_glow_vars::LowHealthHue>()};
            const color::HueInteger highHealthHue{getConfigVariable<model_glow_vars::HighHealthHue>()};
            if (lowHealthHue < highHealthHue)
                return color::Hue{(lowHealthHue + (highHealthHue - lowHealthHue) * fraction) / 360.0f};
            else
                return color::Hue{(lowHealthHue - (lowHealthHue - highHealthHue) * fraction) / 360.0f};
        }
        return {};
    }

    [[nodiscard]] std::optional<color::HueInteger> getPlayerColorHue(auto playerColorIndex) const noexcept
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
