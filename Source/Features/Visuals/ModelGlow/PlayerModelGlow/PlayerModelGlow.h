#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowState.h>

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
        if (state().playerModelGlow == ModelGlowState::State::Disabling)
            state().playerModelGlow = ModelGlowState::State::Disabled;
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
        if (state().playerModelGlow != ModelGlowState::State::Disabled)
            unhookPlayerSceneObjectUpdater(playerPawn);
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return state().playerModelGlow != ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return state().playerModelGlow == ModelGlowState::State::Enabled;
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
        return playerPawn.isAlive().value_or(true)
            && playerPawn.health().greaterThan(0).valueOr(true)
            && !playerPawn.isControlledByLocalPlayer()
            && playerPawn.isTTorCT()
            && (!state().showOnlyEnemies || playerPawn.isEnemy().value_or(true));
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

    [[nodiscard]] cs2::Color getColorSaturated(auto&& playerPawn) const noexcept
    {
        if (state().playerModelGlowColorType == PlayerModelGlowColorType::HealthBased)
            return playerPawn.healthColor(1.0f).value_or(cs2::kColorWhite);

        if (state().playerModelGlowColorType == PlayerModelGlowColorType::PlayerOrTeamColor) {
            if (const auto playerColor = playerPawn.playerController().getPlayerColorSaturated(); playerColor.has_value())
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
        if (state().playerModelGlowColorType == PlayerModelGlowColorType::HealthBased)
            return playerPawn.healthColor(0.5f).value_or(cs2::kColorWhite);

        if (state().playerModelGlowColorType == PlayerModelGlowColorType::PlayerOrTeamColor) {
            if (const auto playerColor = playerPawn.playerController().getPlayerColorHalfSaturated(); playerColor.has_value())
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
