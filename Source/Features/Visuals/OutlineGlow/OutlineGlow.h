#pragma once

#include <utility>

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/Entities/CBaseAnimGraph.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/C_Hostage.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/PlantedC4.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <GameClient/Entities/EntityClassifier.h>

#include "DefuseKitOutlineGlow/DefuseKitOutlineGlow.h"
#include "DroppedBombOutlineGlow/DroppedBombOutlineGlow.h"
#include "GrenadeProjectileOutlineGlow/GrenadeProjectileOutlineGlow.h"
#include "HostageOutlineGlow/HostageOutlineGlow.h"
#include "PlayerOutlineGlow/PlayerOutlineGlow.h"
#include "TickingBombOutlineGlow/TickingBombOutlineGlow.h"
#include "WeaponOutlineGlow/WeaponOutlineGlow.h"

template <typename HookContext>
class OutlineGlow {
public:
    explicit OutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void applyGlowToEntity(EntityTypeInfo entityTypeInfo, auto&& modelEntity) noexcept
    {
        if (!hookContext.config().template getVariable<OutlineGlowEnabled>())
            return;

        if (modelEntity.glowProperty().isGlowing().valueOr(false))
            return;

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            hookContext.template make<PlayerOutlineGlow>().applyGlowToPlayer(modelEntity.template as<PlayerPawn>());
        else if (entityTypeInfo.is<cs2::CBaseAnimGraph>())
            hookContext.template make<DefuseKitOutlineGlow>().applyGlowToDefuseKit(modelEntity.baseEntity());
        else if (entityTypeInfo.is<cs2::CPlantedC4>())
            hookContext.template make<TickingBombOutlineGlow>().applyGlowToPlantedBomb(modelEntity.template as<PlantedC4>());
        else if (entityTypeInfo.is<cs2::C_C4>())
            hookContext.template make<DroppedBombOutlineGlow>().applyGlowToBomb(modelEntity.baseEntity());
        else if (entityTypeInfo.is<cs2::C_Hostage>())
            hookContext.template make<HostageOutlineGlow>().applyGlowToHostage(modelEntity.baseEntity());
        else if (entityTypeInfo.isGrenadeProjectile())
            hookContext.template make<GrenadeProjectileOutlineGlow>().applyGlowToGrenadeProjectile(entityTypeInfo, modelEntity.baseEntity());
        else if (entityTypeInfo.isWeapon())
            hookContext.template make<WeaponOutlineGlow>().applyGlowToWeapon(entityTypeInfo, modelEntity.baseEntity());
    }

    void onUnload() const noexcept
    {
        hookContext.template make<GlowSceneObjects>().clearObjects();
    }

private:
    HookContext& hookContext;
};
