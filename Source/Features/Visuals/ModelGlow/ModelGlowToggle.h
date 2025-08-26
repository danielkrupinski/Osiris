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
        case '0': setVariable<model_glow_vars::Enabled>(true); break;
        case '1': setVariable<model_glow_vars::Enabled>(false); break;
        }
    }

    void updatePlayerModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<model_glow_vars::GlowPlayers>(true); setVariable<model_glow_vars::GlowOnlyEnemies>(true); break;
        case '1': setVariable<model_glow_vars::GlowPlayers>(true); setVariable<model_glow_vars::GlowOnlyEnemies>(false); break;
        case '2': setVariable<model_glow_vars::GlowPlayers>(false); break;
        }
    }

    void updatePlayerModelGlowColor(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::PlayerOrTeamColor); break;
        case '1': setVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::TeamColor); break;
        case '2': setVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::HealthBased); break;
        case '3': setVariable<model_glow_vars::PlayerGlowColorMode>(PlayerModelGlowColorType::EnemyAlly); break;
        }
    }

    void updateWeaponModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<model_glow_vars::GlowWeapons>(true); break;
        case '1': setVariable<model_glow_vars::GlowWeapons>(false); break;
        }
    }

    void updateDroppedBombModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<model_glow_vars::GlowDroppedBomb>(true); break;
        case '1': setVariable<model_glow_vars::GlowDroppedBomb>(false); break;
        }
    }

    void updateTickingBombModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<model_glow_vars::GlowTickingBomb>(true); break;
        case '1': setVariable<model_glow_vars::GlowTickingBomb>(false); break;
        }
    }

    void updateDefuseKitModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<model_glow_vars::GlowDefuseKits>(true); break;
        case '1': setVariable<model_glow_vars::GlowDefuseKits>(false); break;
        }
    }

    void updateGrenadeProjectileModelGlowToggle(char option) noexcept
    {
        switch (option) {
        case '0': setVariable<model_glow_vars::GlowGrenadeProjectiles>(true); break;
        case '1': setVariable<model_glow_vars::GlowGrenadeProjectiles>(false); break;
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
