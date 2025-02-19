#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>

template <typename HookContext>
class GrenadeProjectileModelGlow {
public:
    explicit GrenadeProjectileModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onEntityListTraversed() const noexcept
    {
        if (state().grenadeProjectileModelGlow == ModelGlowState::State::Disabling)
            state().grenadeProjectileModelGlow = ModelGlowState::State::Disabled;
    }

    void updateModelGlow(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        if (isDisabled())
            return;

        if (isEnabled())
            grenadeProjectile.baseEntity().applySpawnProtectionEffectRecursively(getColor(entityTypeInfo));
        else
            grenadeProjectile.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    void onUnload(auto&& grenadeProjectile) const noexcept
    {
        if (!isDisabled())
            grenadeProjectile.baseEntity().removeSpawnProtectionEffectRecursively();
    }

    void enable() const noexcept
    {
        state().grenadeProjectileModelGlow = ModelGlowState::State::Enabled;
    }

    void disable() const noexcept
    {
        if (state().grenadeProjectileModelGlow == ModelGlowState::State::Enabled)
            state().grenadeProjectileModelGlow = ModelGlowState::State::Disabling;
    }

private:
    [[nodiscard]] bool isDisabled() const noexcept
    {
        return state().grenadeProjectileModelGlow == ModelGlowState::State::Disabled;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return state().masterSwitch == ModelGlowState::State::Enabled && state().grenadeProjectileModelGlow == ModelGlowState::State::Enabled;
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    [[nodiscard]] cs2::Color getColor(EntityTypeInfo entityTypeInfo) const noexcept
    {
        using namespace model_glow_params;

        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>(): return kFlashbangColor;
        case EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>(): return kHEGrenadeColor;
        case EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>(): return kMolotovColor;
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>(): return kSmokeGrenadeColor;
        default: return kDefaultWeaponColor;
        }
    }

    HookContext& hookContext;
};
