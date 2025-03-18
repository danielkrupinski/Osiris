#pragma once

#include <GameClient/Entities/EntityClassifier.h>
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

    void applyGlowToPlayer(auto&& playerPawn) const noexcept
    {
        hookContext.template make<PlayerOutlineGlow>().applyGlowToPlayer(playerPawn);
    }

    void applyGlowToWeapon(EntityTypeInfo entityTypeInfo, auto&& modelEntity) const noexcept
    {
        hookContext.template make<WeaponOutlineGlow>().applyGlowToWeapon(entityTypeInfo, modelEntity);
    }

    void applyGlowToDefuseKit(auto&& defuseKit) const noexcept
    {
        hookContext.template make<DefuseKitOutlineGlow>().applyGlowToDefuseKit(defuseKit);
    }

    void applyGlowToGrenadeProjectile(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        hookContext.template make<GrenadeProjectileOutlineGlow>().applyGlowToGrenadeProjectile(entityTypeInfo, grenadeProjectile);
    }
    
    void applyGlowToBomb(auto&& bomb) const noexcept
    {
        hookContext.template make<DroppedBombOutlineGlow>().applyGlowToBomb(bomb);
    }

    void applyGlowToPlantedBomb(auto&& plantedC4) const noexcept
    {
        hookContext.template make<TickingBombOutlineGlow>().applyGlowToPlantedBomb(plantedC4);
    }

    void applyGlowToHostage(auto&& hostage) const noexcept
    {
        hookContext.template make<HostageOutlineGlow>().applyGlowToHostage(hostage);
    }

    [[nodiscard]] auto& viewRenderHook() const noexcept
    {
        return hookContext.hooks().viewRenderHook;
    }

    void clearGlowSceneObjects() const noexcept
    {
        hookContext.template make<GlowSceneObjects>().clearObjects();
    }

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return hookContext.config();
    }

private:
    HookContext& hookContext;
};
