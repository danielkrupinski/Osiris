#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/TeamNumber.h>

#include "ModelGlowState.h"

#include <Entities/PlayerPawn.h>

#include "PlayerModelGlow/PlayerModelGlow.h"
#include "WeaponModelGlow/WeaponModelGlow.h"

template <typename HookContext>
class ModelGlow {
public:
    ModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateSceneObjectUpdaterHook(EntityTypeInfo entityTypeInfo, auto&& entity) const noexcept
    {
        if (!shouldUpdateSceneObjectUpdaterHook())
            return;

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            hookContext.template make<PlayerModelGlow>().updateSceneObjectUpdaterHook(entity.template as<PlayerPawn>());
        else if (entityTypeInfo.isWeapon() && !entityTypeInfo.is<cs2::C_C4>())
            hookContext.template make<WeaponModelGlow>().updateSceneObjectUpdaterHook(entity.template as<BaseWeapon>());
    }

    void applyPlayerModelGlow(auto&& playerPawn) const noexcept
    {
        if (shouldRun())
            hookContext.template make<PlayerModelGlow>().applyModelGlow(playerPawn);
    }

    void applyWeaponModelGlow(auto&& weapon) const noexcept
    {
        if (shouldRun())
            hookContext.template make<WeaponModelGlow>().applyModelGlow(weapon);
    }

    void onEntityListTraversed() const noexcept
    {
        if (state().masterSwitch == ModelGlowState::State::Disabling)
            state().masterSwitch = ModelGlowState::State::Disabled;

        hookContext.template make<PlayerModelGlow>().onEntityListTraversed();
        hookContext.template make<WeaponModelGlow>().onEntityListTraversed();
    }

    void onUnload(EntityTypeInfo entityTypeInfo, auto&& entity) const noexcept
    {
        if (state().masterSwitch == ModelGlowState::State::Disabled)
            return;

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            hookContext.template make<PlayerModelGlow>().onUnload(entity.template as<PlayerPawn>());
        else if (entityTypeInfo.isWeapon() && !entityTypeInfo.is<cs2::C_C4>())
            hookContext.template make<WeaponModelGlow>().onUnload(entity.template as<BaseWeapon>());
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return state().masterSwitch != ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return state().masterSwitch == ModelGlowState::State::Enabled;
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
