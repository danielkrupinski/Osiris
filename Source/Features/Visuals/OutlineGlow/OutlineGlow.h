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

#include "OutlineGlowContext.h"

template <typename HookContext, typename Context = OutlineGlowContext<HookContext>>
class OutlineGlow {
public:
    template <typename... Args>
    OutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToEntity(EntityTypeInfo entityTypeInfo, auto&& modelEntity) noexcept
    {
        if (!context.config().template getVariable<OutlineGlowEnabled>())
            return;

        if (modelEntity.glowProperty().isGlowing().valueOr(false))
            return;

        if (entityTypeInfo.is<cs2::C_CSPlayerPawn>())
            context.applyGlowToPlayer(modelEntity.template as<PlayerPawn>());
        else if (entityTypeInfo.is<cs2::CBaseAnimGraph>())
            context.applyGlowToDefuseKit(modelEntity.baseEntity());
        else if (entityTypeInfo.is<cs2::CPlantedC4>())
            context.applyGlowToPlantedBomb(modelEntity.template as<PlantedC4>());
        else if (entityTypeInfo.is<cs2::C_C4>())
            context.applyGlowToBomb(modelEntity.baseEntity());
        else if (entityTypeInfo.is<cs2::C_Hostage>())
            context.applyGlowToHostage(modelEntity.baseEntity());
        else if (entityTypeInfo.isGrenadeProjectile())
            context.applyGlowToGrenadeProjectile(entityTypeInfo, modelEntity.baseEntity());
        else if (entityTypeInfo.isWeapon())
            context.applyGlowToWeapon(entityTypeInfo, modelEntity.baseEntity());
    }

private:
    Context context;
};
