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
        if (getConfigVariable<PlayerModelGlowEnabled>() || state().playerModelGlowDisabling)
            unhookPlayerSceneObjectUpdater(playerPawn);
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return getConfigVariable<PlayerModelGlowEnabled>() || state().playerModelGlowDisabling;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return getConfigVariable<PlayerModelGlowEnabled>();
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
        return getConfigVariable<ModelGlowEnabled>()
            && playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!getConfigVariable<PlayerModelGlowOnlyEnemies>() || playerPawn.isEnemy().value_or(true));
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
        switch (getConfigVariable<PlayerModelGlowColorMode>()) {
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
        case TeamNumber::TT: return model_glow_params::kTeamTHue.toHueFloat();
        case TeamNumber::CT: return model_glow_params::kTeamCTHue.toHueFloat();
        default: return {};
        }
    }

    [[nodiscard]] std::optional<color::Hue> enemyAllyColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto isEnemy = playerPawn.isEnemy(); isEnemy.has_value())
            return (*isEnemy ? model_glow_params::kEnemyHue : model_glow_params::kAllyHue).toHueFloat();
        return {};
    }

    [[nodiscard]] std::optional<color::Hue> healthBasedColorModeHue(auto&& playerPawn) const noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue())
            return color::Hue{color::kRedHue + (color::kGreenHue - color::kRedHue) * (std::clamp(healthValue.value(), 0, 100) / 100.0f)};
        return {};
    }

    [[nodiscard]] std::optional<color::HueInteger> getPlayerColorHue(auto playerColorIndex) const noexcept
    {
        if (!playerColorIndex.hasValue())
            return {};

        switch (playerColorIndex.value()) {
        case 0: return getConfigVariable<PlayerModelGlowPlayerBlueHue>();
        case 1: return getConfigVariable<PlayerModelGlowPlayerGreenHue>();
        case 2: return getConfigVariable<PlayerModelGlowPlayerYellowHue>();
        case 3: return getConfigVariable<PlayerModelGlowPlayerOrangeHue>();
        case 4: return getConfigVariable<PlayerModelGlowPlayerPurpleHue>();
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
