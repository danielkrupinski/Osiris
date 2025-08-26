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

    void onEntityListTraversed() const noexcept
    {
        state().droppedBombModelGlowDisabling = false;
    }

    void updateSceneObjectUpdaterHook(auto&& bomb) const noexcept
    {
        if (!shouldUpdateSceneObjectUpdaterHook())
            return;

        if (shouldGlowBombModel(bomb))
            hookWeaponSceneObjectUpdater(bomb);
        else
            unhookWeaponSceneObjectUpdater(bomb);
    }

    void applyModelGlow(auto&& bomb) const noexcept
    {
        if (shouldRun() && shouldGlowBombModel(bomb))
            bomb.baseWeapon().baseEntity().applySpawnProtectionEffectRecursively(getColor());
    }

    void onUnload(auto&& bomb) const noexcept
    {
        if (hookContext.config().template getVariable<model_glow_vars::GlowDroppedBomb>() || state().droppedBombModelGlowDisabling)
            unhookWeaponSceneObjectUpdater(bomb);
    }

private:
    [[nodiscard]] cs2::Color getColor() const noexcept
    {
        return color::HSBtoRGB(hookContext.config().template getVariable<model_glow_vars::DroppedBombHue>(), color::Saturation{1.0f}, color::Brightness{1.0f});
    }

    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return hookContext.config().template getVariable<model_glow_vars::GlowDroppedBomb>() || state().droppedBombModelGlowDisabling;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return hookContext.config().template getVariable<model_glow_vars::GlowDroppedBomb>();
    }

    void hookWeaponSceneObjectUpdater(auto&& bomb) const noexcept
    {
        if (!hasSceneObjectUpdaterHooked(bomb)) {
            storeOriginalSceneObjectUpdater(bomb);
            hookSceneObjectUpdater(bomb);
        }
    }

    void unhookWeaponSceneObjectUpdater(auto&& bomb) const noexcept
    {
        if (hasSceneObjectUpdaterHooked(bomb))
            bomb.baseWeapon().setSceneObjectUpdater(state().originalWeaponSceneObjectUpdater);
    }

    void storeOriginalSceneObjectUpdater(auto&& bomb) const noexcept
    {
        if (state().originalWeaponSceneObjectUpdater == nullptr)
            state().originalWeaponSceneObjectUpdater = bomb.baseWeapon().getSceneObjectUpdater();
    }

    [[nodiscard]] bool hasSceneObjectUpdaterHooked(auto&& bomb) const noexcept
    {
        return bomb.baseWeapon().getSceneObjectUpdater() == &Weapon_sceneObjectUpdater_asm;
    }

    void hookSceneObjectUpdater(auto&& bomb) const noexcept
    {
        bomb.baseWeapon().setSceneObjectUpdater(&Weapon_sceneObjectUpdater_asm);
    }

    [[nodiscard]] bool shouldGlowBombModel(auto&& bomb) const noexcept
    {
        return hookContext.config().template getVariable<model_glow_vars::Enabled>() && !bomb.baseWeapon().baseEntity().hasOwner().valueOr(true);
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
