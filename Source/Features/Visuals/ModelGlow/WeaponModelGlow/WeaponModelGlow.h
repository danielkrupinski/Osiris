#pragma once

#include <cstdint>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <Features/Visuals/GrenadePrediction/GrenadeKind.h>
#include <HookContext/HookContextMacros.h>
#include <Platform/Macros/FunctionAttributes.h>

std::uint64_t Weapon_sceneObjectUpdater(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept;

template <typename HookContext>
class WeaponModelGlow {
public:
    explicit WeaponModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(model_glow_vars::GlowWeapons);
    }

    [[nodiscard]] bool shouldApplyGlow(auto&& weapon) const
    {
        return !weapon.baseEntity().hasOwner().valueOr(true);
    }

    [[nodiscard]] auto deactivationFlag() const noexcept
    {
        return ModelGlowDeactivationFlags::WeaponModelGlowDeactivating;
    }

    [[nodiscard]] auto& originalSceneObjectUpdater() const
    {
        return state().originalWeaponSceneObjectUpdater;
    }

    [[nodiscard]] auto replacementSceneObjectUpdater() const
    {
        return &Weapon_sceneObjectUpdater;
    }

    [[nodiscard]] Optional<color::HueInteger> hue(auto&& weapon) const noexcept
    {
        const auto kind = weapon.grenadeKind();
        if (!kind.hasValue())
            return {};
        switch (kind.value()) {
        case cs2::GrenadeKind::Molotov:
        case cs2::GrenadeKind::Incendiary: return GET_CONFIG_VAR(model_glow_vars::MolotovHue);
        case cs2::GrenadeKind::Flashbang: return GET_CONFIG_VAR(model_glow_vars::FlashbangHue);
        case cs2::GrenadeKind::HEGrenade: return GET_CONFIG_VAR(model_glow_vars::HEGrenadeHue);
        case cs2::GrenadeKind::SmokeGrenade: return GET_CONFIG_VAR(model_glow_vars::SmokeGrenadeHue);
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
