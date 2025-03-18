#pragma once

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/Entities/CBaseAnimGraph.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/BaseWeapon.h>
#include <GameClient/Entities/C4.h>
#include <GameClient/Entities/PlantedC4.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/Entities/EntityClassifier.h>

#include "DefuseKitModelGlow/DefuseKitModelGlow.h"
#include "DroppedBombModelGlow/DroppedBombModelGlow.h"
#include "GrenadeProjectileModelGlow/GrenadeProjectileModelGlow.h"
#include "ModelGlowState.h"
#include "PlayerModelGlow/PlayerModelGlow.h"
#include "TickingBombModelGlow/TickingBombModelGlow.h"
#include "WeaponModelGlow/WeaponModelGlow.h"

template <typename HookContext>
class ModelGlow {
public:
    ModelGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateSceneObjectUpdaterHook(EntityTypeInfo entityTypeInfo, auto&& entity) const noexcept
    {
        if (!shouldUpdateSceneObjectUpdaterHook())
            return;

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            hookContext.template make<PlayerModelGlow>().updateSceneObjectUpdaterHook(entity.template as<PlayerPawn>());
        else if (entityTypeInfo.is<cs2::C_C4>())
            hookContext.template make<DroppedBombModelGlow>().updateSceneObjectUpdaterHook(entity.template as<C4>());
        else if (entityTypeInfo.is<cs2::CBaseAnimGraph>())
            hookContext.template make<DefuseKitModelGlow>().updateModelGlow(entity);
        else if (entityTypeInfo.is<cs2::CPlantedC4>())
            hookContext.template make<TickingBombModelGlow>().applyModelGlow(entity.template as<PlantedC4>());
        else if (entityTypeInfo.isGrenadeProjectile())
            hookContext.template make<GrenadeProjectileModelGlow>().updateModelGlow(entityTypeInfo, entity);
        else if (entityTypeInfo.isWeapon())
            hookContext.template make<WeaponModelGlow>().updateSceneObjectUpdaterHook(entity.template as<BaseWeapon>());
    }

    void updateSceneObjectUpdaterHook(auto&& playerPawnForModelGlowPreview) const noexcept
    {
        if (shouldUpdateSceneObjectUpdaterHook())
            hookContext.template make<PlayerModelGlow>().updateSceneObjectUpdaterHook(playerPawnForModelGlowPreview);
    }

    void applyPlayerModelGlow(auto&& playerPawn) const noexcept
    {
        if (shouldRun())
            hookContext.template make<PlayerModelGlow>().applyModelGlow(playerPawn);
    }

    void applyWeaponModelGlow(auto&& weapon) const noexcept
    {
        if (!shouldRun())
            return;

        if (auto&& c4 = weapon.template cast<C4>())
            hookContext.template make<DroppedBombModelGlow>().applyModelGlow(c4);
        else
            hookContext.template make<WeaponModelGlow>().applyModelGlow(weapon);
    }

    void onEntityListTraversed() const noexcept
    {
        state().modelGlowDisabling = false;
        hookContext.template make<PlayerModelGlow>().onEntityListTraversed();
        hookContext.template make<WeaponModelGlow>().onEntityListTraversed();
        hookContext.template make<DroppedBombModelGlow>().onEntityListTraversed();
        hookContext.template make<TickingBombModelGlow>().onEntityListTraversed();
        hookContext.template make<DefuseKitModelGlow>().onEntityListTraversed();
        hookContext.template make<GrenadeProjectileModelGlow>().onEntityListTraversed();
    }

    void onUnload(EntityTypeInfo entityTypeInfo, auto&& entity) const noexcept
    {
        if (!hookContext.config().template getVariable<ModelGlowEnabled>() && !state().modelGlowDisabling)
            return;

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            hookContext.template make<PlayerModelGlow>().onUnload(entity.template as<PlayerPawn>());
        else if (entityTypeInfo.is<cs2::C_C4>())
            hookContext.template make<DroppedBombModelGlow>().onUnload(entity.template as<C4>());
        else if (entityTypeInfo.is<cs2::CBaseAnimGraph>())
            hookContext.template make<DefuseKitModelGlow>().onUnload(entity);
        else if (entityTypeInfo.is<cs2::CPlantedC4>())
            hookContext.template make<TickingBombModelGlow>().onUnload(entity.template as<PlantedC4>());
        else if (entityTypeInfo.isGrenadeProjectile())
            hookContext.template make<GrenadeProjectileModelGlow>().onUnload(entity);
        else if (entityTypeInfo.isWeapon())
            hookContext.template make<WeaponModelGlow>().onUnload(entity.template as<BaseWeapon>());
    }

private:
    [[nodiscard]] bool shouldUpdateSceneObjectUpdaterHook() const noexcept
    {
        return hookContext.config().template getVariable<ModelGlowEnabled>() || state().modelGlowDisabling;
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return hookContext.config().template getVariable<ModelGlowEnabled>();
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.modelGlowState;
    }

    HookContext& hookContext;
};
