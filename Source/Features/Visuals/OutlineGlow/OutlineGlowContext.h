#pragma once

#include <GameClasses/PlayerPawn.h>
#include <GameClasses/PlantedC4.h>
#include <OutlineGlow/GlowSceneObjects.h>

#include "DefuseKitOutlineGlow/DefuseKitOutlineGlow.h"
#include "DroppedBombOutlineGlow/DroppedBombOutlineGlow.h"
#include "GrenadeProjectileOutlineGlow/GrenadeProjectileOutlineGlow.h"
#include "HostageOutlineGlow/HostageOutlineGlow.h"
#include "PlayerOutlineGlow/PlayerOutlineGlow.h"
#include "TickingBombOutlineGlow/TickingBombOutlineGlow.h"
#include "WeaponOutlineGlow/WeaponOutlineGlow.h"

template <typename HookContext>
class OutlineGlowContext {
public:
    explicit OutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    void applyGlowToPlayer(auto& entity) const noexcept
    {
        hookContext.template make<PlayerOutlineGlow>().applyGlowToPlayer(hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(&entity)));
    }

    void applyGlowToWeapon(EntityTypeInfo entityTypeInfo, auto& entity) const noexcept
    {
        hookContext.template make<WeaponOutlineGlow>().applyGlowToWeapon(entityTypeInfo, hookContext.template make<BaseEntity>(static_cast<cs2::C_CSWeaponBase*>(&entity)));
    }

    void applyGlowToDefuseKit(auto& entity) const noexcept
    {
        hookContext.template make<DefuseKitOutlineGlow>().applyGlowToDefuseKit(hookContext.template make<BaseEntity>(static_cast<cs2::CBaseAnimGraph*>(&entity)));
    }

    void applyGlowToGrenadeProjectile(EntityTypeInfo entityTypeInfo, auto& entity) const noexcept
    {
        hookContext.template make<GrenadeProjectileOutlineGlow>().applyGlowToGrenadeProjectile(entityTypeInfo, hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseCSGrenadeProjectile*>(&entity)));
    }
    
    void applyGlowToBomb(auto& entity) const noexcept
    {
        hookContext.template make<DroppedBombOutlineGlow>().applyGlowToBomb(hookContext.template make<BaseEntity>(static_cast<cs2::C_C4*>(&entity)));
    }

    void applyGlowToPlantedBomb(auto& entity) const noexcept
    {
        hookContext.template make<TickingBombOutlineGlow>().applyGlowToPlantedBomb(hookContext.template make<PlantedC4<HookContext>>(static_cast<cs2::CPlantedC4*>(&entity)));
    }

    void applyGlowToHostage(auto& entity) const noexcept
    {
        hookContext.template make<HostageOutlineGlow>().applyGlowToHostage(hookContext.template make<BaseEntity>(static_cast<cs2::C_Hostage*>(&entity)));
    }

    [[nodiscard]] auto& viewRenderHook() const noexcept
    {
        return hookContext.hooks().viewRenderHook;
    }

    void clearGlowSceneObjects() const noexcept
    {
        hookContext.template make<GlowSceneObjects>().clearObjects();
    }

private:
    HookContext& hookContext;
};
