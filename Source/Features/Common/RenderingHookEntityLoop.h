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

        if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>()) {
            hookContext.template make<PlayerInfoInWorld>().drawPlayerInformation(baseEntity.template as<PlayerPawn>());
            updateModelGlow<PlayerModelGlow>(baseEntity.template as<PlayerPawn>(), entityTypeInfo);
            applyOutlineGlow<PlayerOutlineGlow>(baseEntity.template as<PlayerPawn>(), entityTypeInfo);
        } else if (entityTypeInfo.template is<cs2::C_C4>()) {
            updateModelGlow<DroppedBombModelGlow>(baseEntity.template as<BaseWeapon>(), entityTypeInfo);
            applyOutlineGlow<DroppedBombOutlineGlow>(baseEntity, entityTypeInfo);
        } else if (entityTypeInfo.template is<cs2::CBaseAnimGraph>()) {
            updateModelGlow<DefuseKitModelGlow>(baseEntity, entityTypeInfo);
            applyOutlineGlow<DefuseKitOutlineGlow>(baseEntity, entityTypeInfo);
        } else if (entityTypeInfo.template is<cs2::CPlantedC4>()) {
            updateModelGlow<TickingBombModelGlow>(baseEntity.template as<PlantedC4>(), entityTypeInfo);
            applyOutlineGlow<TickingBombOutlineGlow>(baseEntity.template as<PlantedC4>(), entityTypeInfo);
        }  else if (entityTypeInfo.template is<cs2::C_Hostage>()) {
            applyOutlineGlow<HostageOutlineGlow>(baseEntity, entityTypeInfo);
        } else if (entityTypeInfo.isGrenadeProjectile()) {
            updateModelGlow<GrenadeProjectileModelGlow>(baseEntity, entityTypeInfo);
            applyOutlineGlow<GrenadeProjectileOutlineGlow>(baseEntity, entityTypeInfo);
        } else if (entityTypeInfo.isWeapon()) {
            updateModelGlow<WeaponModelGlow>(baseEntity.template as<BaseWeapon>(), entityTypeInfo);
            applyOutlineGlow<WeaponOutlineGlow>(baseEntity, entityTypeInfo);
        }
    }

    template <template <typename> typename Glow, typename... Args>
    void updateModelGlow(Args&&... args) const
    {
        hookContext.template make<ModelGlow>().updateInMainThread()(Glow{hookContext}, std::forward<Args>(args)...);
    }

    template <template <typename> typename Glow, typename... Args>
    void applyOutlineGlow(Args&&... args) const
    {
        hookContext.template make<OutlineGlow>().applyGlow()(Glow{hookContext}, std::forward<Args>(args)...);
    }

    HookContext& hookContext;
};
