#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>

extern "C" std::uint64_t Weapon_sceneObjectUpdater_asm(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept;

template <typename HookContext>
class DroppedBombModelGlow {
public:
    explicit DroppedBombModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return hookContext.config().template getVariable<model_glow_vars::GlowDroppedBomb>();
    }

    [[nodiscard]] bool shouldApplyGlow(auto&& bomb) const
    {
        return !bomb.baseEntity().hasOwner().valueOr(true);
    }

    [[nodiscard]] bool& disablingFlag() const
    {
        return state().droppedBombModelGlowDisabling;
    }

    [[nodiscard]] auto& originalSceneObjectUpdater() const
    {
        return state().originalWeaponSceneObjectUpdater;
    }

    [[nodiscard]] auto replacementSceneObjectUpdater() const
    {
        return &Weapon_sceneObjectUpdater_asm;
    }

    [[nodiscard]] color::HueInteger hue() const
    {
        return hookContext.config().template getVariable<model_glow_vars::DroppedBombHue>();
    }

private:
    [[nodiscard]] auto& state() const
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
