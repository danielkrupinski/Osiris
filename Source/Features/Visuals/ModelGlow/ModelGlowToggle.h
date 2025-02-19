#pragma once

#include "ModelGlowState.h"

// todo: Move to GUI
template <typename HookContext>
class ModelGlowToggle {
public:
    ModelGlowToggle(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateMasterSwitch(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.template make<ModelGlow>().enable(); break;
        case '1': hookContext.template make<ModelGlow>().disable(); break;
        }
    }

    void updatePlayerModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.template make<PlayerModelGlow>().enable(); state().showOnlyEnemies = true; break;
        case '1': hookContext.template make<PlayerModelGlow>().enable(); state().showOnlyEnemies = false; break;
        case '2': hookContext.template make<PlayerModelGlow>().disable(); break;
        }
    }

    void updatePlayerModelGlowColor(char option) noexcept
    {
        switch (option) {
        case '0': state().playerModelGlowColorType = PlayerModelGlowColorType::PlayerOrTeamColor; break;
        case '1': state().playerModelGlowColorType = PlayerModelGlowColorType::TeamColor; break;
        case '2': state().playerModelGlowColorType = PlayerModelGlowColorType::HealthBased; break;
        }
    }

    void updateWeaponModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.template make<WeaponModelGlow>().enable(); break;
        case '1': hookContext.template make<WeaponModelGlow>().disable(); break;
        }
    }

    void updateDroppedBombModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.template make<DroppedBombModelGlow>().enable(); break;
        case '1': hookContext.template make<DroppedBombModelGlow>().disable(); break;
        }
    }

    void updateTickingBombModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.template make<TickingBombModelGlow>().enable(); break;
        case '1': hookContext.template make<TickingBombModelGlow>().disable(); break;
        }
    }

    void updateDefuseKitModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.template make<DefuseKitModelGlow>().enable(); break;
        case '1': hookContext.template make<DefuseKitModelGlow>().disable(); break;
        }
    }

    void updateGrenadeProjectileModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': hookContext.template make<GrenadeProjectileModelGlow>().enable(); break;
        case '1': hookContext.template make<GrenadeProjectileModelGlow>().disable(); break;
        }
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
