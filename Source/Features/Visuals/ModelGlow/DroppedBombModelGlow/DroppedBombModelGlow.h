#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <HookContext/HookContextMacros.h>

std::uint64_t Weapon_sceneObjectUpdater(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept;

template <typename HookContext>
class DroppedBombModelGlow {
public:
    explicit DroppedBombModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(model_glow_vars::GlowDroppedBomb);
    }

    [[nodiscard]] bool shouldApplyGlow(auto&& bomb) const
    {
        return !bomb.baseEntity().hasOwner().valueOr(true);
    }

    [[nodiscard]] auto deactivationFlag() const noexcept
    {
        return ModelGlowDeactivationFlags::DroppedBombModelGlowDeactivating;
    }

    [[nodiscard]] auto& originalSceneObjectUpdater() const
    {
        return state().originalWeaponSceneObjectUpdater;
    }

    [[nodiscard]] auto replacementSceneObjectUpdater() const
    {
        return &Weapon_sceneObjectUpdater;
    }

    [[nodiscard]] color::HueInteger hue() const
    {
        return GET_CONFIG_VAR(model_glow_vars::DroppedBombHue);
    }

private:
    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
