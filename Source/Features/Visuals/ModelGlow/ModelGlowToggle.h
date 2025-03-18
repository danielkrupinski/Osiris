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
        case '0': setVariable<ModelGlowEnabled>(true); break;
        case '1': setVariable<ModelGlowEnabled>(false); break;
        }
    }

    void updatePlayerModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<PlayerModelGlowEnabled>(true); setVariable<PlayerModelGlowOnlyEnemies>(true); break;
        case '1': setVariable<PlayerModelGlowEnabled>(true); setVariable<PlayerModelGlowOnlyEnemies>(false); break;
        case '2': setVariable<PlayerModelGlowEnabled>(false); break;
        }
    }

    void updatePlayerModelGlowColor(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<PlayerModelGlowColorMode>(PlayerModelGlowColorType::PlayerOrTeamColor); break;
        case '1': setVariable<PlayerModelGlowColorMode>(PlayerModelGlowColorType::TeamColor); break;
        case '2': setVariable<PlayerModelGlowColorMode>(PlayerModelGlowColorType::HealthBased); break;
        }
    }

    void updateWeaponModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<WeaponModelGlowEnabled>(true); break;
        case '1': setVariable<WeaponModelGlowEnabled>(false); break;
        }
    }

    void updateDroppedBombModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<DroppedBombModelGlowEnabled>(true); break;
        case '1': setVariable<DroppedBombModelGlowEnabled>(false); break;
        }
    }

    void updateTickingBombModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<TickingBombModelGlowEnabled>(true); break;
        case '1': setVariable<TickingBombModelGlowEnabled>(false); break;
        }
    }

    void updateDefuseKitModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<DefuseKitModelGlowEnabled>(true); break;
        case '1': setVariable<DefuseKitModelGlowEnabled>(false); break;
        }
    }

    void updateGrenadeProjectileModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<GrenadeProjectileModelGlowEnabled>(true); break;
        case '1': setVariable<GrenadeProjectileModelGlowEnabled>(false); break;
        }
    }

private:
    template <typename ConfigVariable>
    void setVariable(ConfigVariable::ValueType value) noexcept
    {
        hookContext.config().template setVariable<ConfigVariable>(value);
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
