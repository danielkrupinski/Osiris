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

    [[nodiscard]] bool enabled() const
    {
        return hookContext.config().template getVariable<model_glow_vars::GlowGrenadeProjectiles>();
    }

    [[nodiscard]] bool& disablingFlag() const
    {
        return state().grenadeProjectileModelGlowDisabling;
    }

    [[nodiscard]] std::optional<color::HueInteger> getGlowHue(EntityTypeInfo entityTypeInfo) const noexcept
    {
        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>(): return hookContext.config().template getVariable<model_glow_vars::FlashbangHue>();
        case EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>(): return hookContext.config().template getVariable<model_glow_vars::HEGrenadeHue>();
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>(): return hookContext.config().template getVariable<model_glow_vars::SmokeGrenadeHue>();
        case EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>(): return hookContext.config().template getVariable<model_glow_vars::MolotovHue>();
        default: return {};
        }
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
