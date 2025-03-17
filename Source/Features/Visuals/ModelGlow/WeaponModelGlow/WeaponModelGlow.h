#pragma once

#include <cstdint>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Features/Visuals/ModelGlow/ModelGlowParams.h>
#include <Features/Visuals/ModelGlow/ModelGlowState.h>

extern "C" std::uint64_t Weapon_sceneObjectUpdater_asm(cs2::C_CSWeaponBase* weapon, void* unknown, bool unknownBool) noexcept;

template <typename HookContext>
class WeaponModelGlow {
public:
    explicit WeaponModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onEntityListTraversed() const noexcept
    {
        state().weaponModelGlowDisabling = true;
    }

    void updateSceneObjectUpdaterHook(auto&& weapon) const noexcept
    {
        if (!shouldUpdateSceneObjectUpdaterHook())
            return;

        if (shouldGlowWeaponModel(weapon))
            hookWeaponSceneObjectUpdater(weapon);
        else
            unhookWeaponSceneObjectUpdater(weapon);
    }

    void applyModelGlow(auto&& weapon) const noexcept
    {
        if (shouldRun() && shouldGlowWeaponModel(weapon))
            weapon.baseEntity().applySpawnProtectionEffectRecursively(getColor(weapon.baseEntity().classify()));
    }

    void onUnload(auto&& weapon) const noexcept
    {
        if (hookContext.config().template getVariable<WeaponModelGlowEnabled>() || state().weaponModelGlowDisabling)
            unhookWeaponSceneObjectUpdater(weapon);
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return hookContext.config().template getVariable<WeaponModelGlowEnabled>() || state().weaponModelGlowDisabling;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return hookContext.config().template getVariable<WeaponModelGlowEnabled>();
    }

    void hookWeaponSceneObjectUpdater(auto&& weapon) const noexcept
    {
        if (!hasSceneObjectUpdaterHooked(weapon)) {
            storeOriginalSceneObjectUpdater(weapon);
            hookSceneObjectUpdater(weapon);
        }
    }

    void unhookWeaponSceneObjectUpdater(auto&& weapon) const noexcept
    {
        if (hasSceneObjectUpdaterHooked(weapon))
            weapon.setSceneObjectUpdater(state().originalWeaponSceneObjectUpdater);
    }

    void storeOriginalSceneObjectUpdater(auto&& weapon) const noexcept
    {
        if (state().originalWeaponSceneObjectUpdater == nullptr)
            state().originalWeaponSceneObjectUpdater = weapon.getSceneObjectUpdater();
    }

    [[nodiscard]] bool hasSceneObjectUpdaterHooked(auto&& weapon) const noexcept
    {
        return weapon.getSceneObjectUpdater() == &Weapon_sceneObjectUpdater_asm;
    }

    void hookSceneObjectUpdater(auto&& weapon) const noexcept
    {
        weapon.setSceneObjectUpdater(&Weapon_sceneObjectUpdater_asm);
    }

    [[nodiscard]] bool shouldGlowWeaponModel(auto&& weapon) const noexcept
    {
        return hookContext.config().template getVariable<ModelGlowEnabled>() && !weapon.baseEntity().hasOwner().valueOr(true);
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    [[nodiscard]] cs2::Color getColor(EntityTypeInfo entityTypeInfo) const noexcept
    {
        using namespace model_glow_params;
        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>():
        case EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>(): return kMolotovColor;
        case EntityTypeInfo::indexOf<cs2::C_Flashbang>(): return kFlashbangColor;
        case EntityTypeInfo::indexOf<cs2::C_HEGrenade>(): return kHEGrenadeColor;
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>(): return kSmokeGrenadeColor;
        default: return kDefaultWeaponColor;
        }
    }

    HookContext& hookContext;
};
