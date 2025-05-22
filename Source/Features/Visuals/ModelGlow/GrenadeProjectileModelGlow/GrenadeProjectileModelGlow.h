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
        state().grenadeProjectileModelGlowDisabling = false;
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

private:
    [[nodiscard]] bool isDisabled() const noexcept
    {
        return !hookContext.config().template getVariable<GrenadeProjectileModelGlowEnabled>() && !state().grenadeProjectileModelGlowDisabling;
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return hookContext.config().template getVariable<ModelGlowEnabled>() && hookContext.config().template getVariable<GrenadeProjectileModelGlowEnabled>();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    [[nodiscard]] std::optional<color::HueInteger> getHue(EntityTypeInfo entityTypeInfo) const noexcept
    {
        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>(): return hookContext.config().template getVariable<ModelGlowFlashbangHue>();
        case EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>(): return hookContext.config().template getVariable<ModelGlowHEGrenadeHue>();
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>(): return hookContext.config().template getVariable<ModelGlowSmokeGrenadeHue>();
        case EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>(): return hookContext.config().template getVariable<ModelGlowMolotovHue>();
        default: return {};
        }
    }

    [[nodiscard]] cs2::Color getColor(EntityTypeInfo entityTypeInfo) const noexcept
    {
        if (const auto hue = getHue(entityTypeInfo); hue.has_value())
            return color::HSBtoRGB(*hue, color::Saturation{1.0f}, color::Brightness{1.0f});
        return model_glow_params::kDefaultWeaponColor;
    }

    HookContext& hookContext;
};
