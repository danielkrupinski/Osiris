#pragma once

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <GameClient/Entities/BaseEntity.h>
#include <GameClient/Entities/BaseModelEntity.h>
#include <GameClient/Entities/PlayerPawn.h>
#include <Features/Visuals/ModelGlow/ModelGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlow.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorld.h>
#include <GameClient/EntitySystem/EntitySystem.h>

template <typename HookContext>
class RenderingHookEntityLoop {
public:
    explicit RenderingHookEntityLoop(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void run() const noexcept
    {
        hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity([this](const auto& entityIdentity) { handleEntityIdentity(entityIdentity); });
        hookContext.template make<ModelGlow>().postUpdateInMainThread();
    }

private:
    void handleEntityIdentity(const cs2::CEntityIdentity& entityIdentity) const noexcept
    {
        const auto entityTypeInfo = hookContext.entityClassifier().classifyEntity(entityIdentity.entityClass);
        auto&& baseEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(entityIdentity.entity));

        if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>())
            hookContext.template make<PlayerInfoInWorld>().drawPlayerInformation(baseEntity.template as<PlayerPawn>());

        if (entityTypeInfo.isModelEntity())
            hookContext.template make<OutlineGlow>().applyGlowToEntity(entityTypeInfo, baseEntity.template as<BaseModelEntity>());

        if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>())
            hookContext.template make<ModelGlow>().updateInMainThread()(PlayerModelGlow{hookContext}, baseEntity.template as<PlayerPawn>(), entityTypeInfo);
        else if (entityTypeInfo.template is<cs2::C_C4>())
            hookContext.template make<ModelGlow>().updateInMainThread()(DroppedBombModelGlow{hookContext}, baseEntity.template as<BaseWeapon>(), entityTypeInfo);
        else if (entityTypeInfo.template is<cs2::CBaseAnimGraph>())
            hookContext.template make<ModelGlow>().updateInMainThread()(DefuseKitModelGlow{hookContext}, baseEntity, entityTypeInfo);
        else if (entityTypeInfo.template is<cs2::CPlantedC4>())
            hookContext.template make<ModelGlow>().updateInMainThread()(TickingBombModelGlow{hookContext}, baseEntity.template as<PlantedC4>(), entityTypeInfo);
        else if (entityTypeInfo.isGrenadeProjectile())
            hookContext.template make<ModelGlow>().updateInMainThread()(GrenadeProjectileModelGlow{hookContext}, baseEntity, entityTypeInfo);
        else if (entityTypeInfo.isWeapon())
            hookContext.template make<ModelGlow>().updateInMainThread()(WeaponModelGlow{hookContext}, baseEntity.template as<BaseWeapon>(), entityTypeInfo);
    }

    HookContext& hookContext;
};
