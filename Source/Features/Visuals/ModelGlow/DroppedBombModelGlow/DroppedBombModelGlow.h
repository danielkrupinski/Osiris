#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowState.h>

#include "DroppedBombModelGlowParams.h"

extern "C" std::uint64_t Weapon_sceneObjectUpdater_asm(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept;

template <typename HookContext>
class DroppedBombModelGlow {
public:
    explicit DroppedBombModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onEntityListTraversed() const noexcept
    {
        if (state().droppedBombModelGlow == ModelGlowState::State::Disabling)
            state().droppedBombModelGlow = ModelGlowState::State::Disabled;
    }

    void updateSceneObjectUpdaterHook(auto&& bomb) const noexcept
    {
        if (!shouldUpdateSceneObjectUpdaterHook())
            return;

        if (shouldGlowBombModel(bomb))
            hookWeaponSceneObjectUpdater(bomb);
        else
            unhookWeaponSceneObjectUpdater(bomb);
    }

    void applyModelGlow(auto&& bomb) const noexcept
    {
        if (shouldRun() && shouldGlowBombModel(bomb))
            bomb.baseWeapon().baseEntity().applySpawnProtectionEffectRecursively(dropped_bomb_model_glow_params::kDroppedBombColor);
    }

    void onUnload(auto&& bomb) const noexcept
    {
        if (state().droppedBombModelGlow != ModelGlowState::State::Disabled)
            unhookWeaponSceneObjectUpdater(bomb);
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return state().droppedBombModelGlow != ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return state().droppedBombModelGlow == ModelGlowState::State::Enabled;
    }

    void hookWeaponSceneObjectUpdater(auto&& bomb) const noexcept
    {
        if (!hasSceneObjectUpdaterHooked(bomb)) {
            storeOriginalSceneObjectUpdater(bomb);
            hookSceneObjectUpdater(bomb);
        }
    }

    void unhookWeaponSceneObjectUpdater(auto&& bomb) const noexcept
    {
        if (hasSceneObjectUpdaterHooked(bomb))
            bomb.baseWeapon().setSceneObjectUpdater(state().originalWeaponSceneObjectUpdater);
    }

    void storeOriginalSceneObjectUpdater(auto&& bomb) const noexcept
    {
        if (state().originalWeaponSceneObjectUpdater == nullptr)
            state().originalWeaponSceneObjectUpdater = bomb.baseWeapon().getSceneObjectUpdater();
    }

    [[nodiscard]] bool hasSceneObjectUpdaterHooked(auto&& bomb) const noexcept
    {
        return bomb.baseWeapon().getSceneObjectUpdater() == &Weapon_sceneObjectUpdater_asm;
    }

    void hookSceneObjectUpdater(auto&& bomb) const noexcept
    {
        bomb.baseWeapon().setSceneObjectUpdater(&Weapon_sceneObjectUpdater_asm);
    }

    [[nodiscard]] bool shouldGlowBombModel(auto&& bomb) const noexcept
    {
        return !bomb.baseWeapon().baseEntity().hasOwner().valueOr(true);
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
