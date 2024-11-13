#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/TeamNumber.h>

#include "ModelGlowState.h"

extern "C" std::uint64_t PlayerPawn_sceneObjectUpdater_asm(cs2::C_CSPlayerPawn* playerPawn) noexcept;
extern "C" std::uint64_t PlayerPawn_sceneObjectUpdater_cpp(cs2::C_CSPlayerPawn* playerPawn) noexcept;

template <typename HookContext>
class ModelGlow {
public:
    ModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
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

    void onEntityListTraversed() const noexcept
    {
        if (state().masterSwitch == ModelGlowState::State::Disabling) {
            state().masterSwitch = ModelGlowState::State::Disabled;
            hookContext.hooks().viewRenderHook.decrementReferenceCount();
        }

        if (state().playerModelGlow == ModelGlowState::State::Disabling)
            state().playerModelGlow = ModelGlowState::State::Disabled;
    }

    void onUnload(auto&& playerPawn) const noexcept
    {
        if (state().masterSwitch != ModelGlowState::State::Disabled && state().playerModelGlow != ModelGlowState::State::Disabled)
            unhookPlayerSceneObjectUpdater(playerPawn);
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return state().masterSwitch != ModelGlowState::State::Disabled && state().playerModelGlow != ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return state().masterSwitch == ModelGlowState::State::Enabled && state().playerModelGlow == ModelGlowState::State::Enabled;
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
        return playerPawn.getSceneObjectUpdater() == &WIN64_LINUX(PlayerPawn_sceneObjectUpdater_asm, PlayerPawn_sceneObjectUpdater_cpp);
    }

    void hookSceneObjectUpdater(auto&& playerPawn) const noexcept
    {
        playerPawn.setSceneObjectUpdater(&WIN64_LINUX(PlayerPawn_sceneObjectUpdater_asm, PlayerPawn_sceneObjectUpdater_cpp));
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
        if (playerPawn.hasImmunity().valueOr(false)) {
            switch (playerPawn.teamNumber()) {
            case TeamNumber::TT: return cs2::Color{255, 217, 128};
            case TeamNumber::CT: return cs2::Color{128, 191, 255};
            default: return cs2::kColorWhite;
            }
        }

        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return cs2::Color{255, 179, 0};
        case TeamNumber::CT: return cs2::Color{0, 127, 255};
        default: return cs2::kColorWhite;
        }
    }

    HookContext& hookContext;
};
