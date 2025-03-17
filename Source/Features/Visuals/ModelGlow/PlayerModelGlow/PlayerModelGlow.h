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
        if (hookContext.config().template getVariable<PlayerModelGlowEnabled>() || state().playerModelGlowDisabling)
            unhookPlayerSceneObjectUpdater(playerPawn);
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return hookContext.config().template getVariable<PlayerModelGlowEnabled>() || state().playerModelGlowDisabling;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return hookContext.config().template getVariable<PlayerModelGlowEnabled>();
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
        return hookContext.config().template getVariable<ModelGlowEnabled>()
            && playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!hookContext.config().template getVariable<PlayerModelGlowOnlyEnemies>() || playerPawn.isEnemy().value_or(true));
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        if (playerPawn.hasImmunity().valueOr(false))
            return getColorHalfSaturated(playerPawn);
        return getColorSaturated(playerPawn);
    }

    [[nodiscard]] std::optional<cs2::Color> getPlayerColor(auto playerColorIndex) const noexcept
    {
        return getPlayerColor(playerColorIndex, cs2::kPlayerColors);
    }

    [[nodiscard]] std::optional<cs2::Color> getPlayerColorSaturated(auto playerColorIndex) const noexcept
    {
        return getPlayerColor(playerColorIndex, cs2::kPlayerColorsSaturated);
    }

    [[nodiscard]] std::optional<cs2::Color> getPlayerColorHalfSaturated(auto playerColorIndex) const noexcept
    {
        return getPlayerColor(playerColorIndex, cs2::kPlayerColorsHalfSaturated);
    }

    [[nodiscard]] std::optional<cs2::Color> getPlayerColor(auto playerColorIndex, std::span<const cs2::Color> playerColors) const noexcept
    {
        if (playerColorIndex.hasValue() && playerColorIndex.value() >= 0 && std::cmp_less(playerColorIndex.value(), playerColors.size()))
            return playerColors[playerColorIndex.value()];
        return {};
    }

    [[nodiscard]] std::optional<cs2::Color> healthColor(auto&& playerPawn, float saturation = 0.7f) const noexcept
    {
        if (const auto healthValue = playerPawn.health(); healthValue.hasValue())
            return getColorOfHealthFraction(saturation, std::clamp(healthValue.value(), 0, 100) / 100.0f);
        return {};
    }
    [[nodiscard]] static cs2::Color getColorOfHealthFraction(float saturation, float healthFraction) noexcept
    {
        return color::HSBtoRGB(color::kRedHue + (color::kGreenHue - color::kRedHue) * healthFraction, saturation, 1.0f);
    }

    [[nodiscard]] cs2::Color getColorSaturated(auto&& playerPawn) const noexcept
    {
        if (hookContext.config().template getVariable<PlayerModelGlowColorMode>() == PlayerModelGlowColorType::HealthBased)
            return healthColor(playerPawn, 1.0f).value_or(cs2::kColorWhite);

        if (hookContext.config().template getVariable<PlayerModelGlowColorMode>() == PlayerModelGlowColorType::PlayerOrTeamColor) {
            if (const auto playerColor = getPlayerColorSaturated(playerPawn.playerController().playerColorIndex()); playerColor.has_value())
                return *playerColor;
        }

        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return cs2::Color{255, 179, 0};
        case TeamNumber::CT: return cs2::Color{0, 127, 255};
        default: return cs2::kColorWhite;
        }
    }

    [[nodiscard]] cs2::Color getColorHalfSaturated(auto&& playerPawn) const noexcept
    {
        if (hookContext.config().template getVariable<PlayerModelGlowColorMode>() == PlayerModelGlowColorType::HealthBased)
            return healthColor(playerPawn, 0.5f).value_or(cs2::kColorWhite);

        if (hookContext.config().template getVariable<PlayerModelGlowColorMode>() == PlayerModelGlowColorType::PlayerOrTeamColor) {
            if (const auto playerColor = getPlayerColorHalfSaturated(playerPawn.playerController().playerColorIndex()); playerColor.has_value())
                return *playerColor;
        }

        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return cs2::Color{255, 217, 128};
        case TeamNumber::CT: return cs2::Color{128, 191, 255};
        default: return cs2::kColorWhite;
        }
    }

    HookContext& hookContext;
};
