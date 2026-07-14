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
#include <Features/Hud/BombPlantAlert/BombPlantAlert.h>

template <typename HookContext>
class RenderingHookEntityLoop {
public:
    explicit RenderingHookEntityLoop(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void run() const noexcept
    {
        auto bombPlantAlertVisibility = Visibility::Hidden;
        hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity([this, &bombPlantAlertVisibility](const auto& entityIdentity) { handleEntityIdentity(entityIdentity, bombPlantAlertVisibility); });
        hookContext.template make<ModelGlow>().postUpdateInMainThread();
        if (bombPlantAlertVisibility == Visibility::Hidden)
            hookContext.template make<BombPlantAlert>().hide();
    }

private:
    void handleEntityIdentity(const cs2::CEntityIdentity& entityIdentity, Visibility& bombPlantAlertVisibility) const noexcept
    {
        const auto entityTypeInfo = hookContext.entityClassifier().classifyEntity(entityIdentity.entityClass);
        auto&& baseEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(entityIdentity.entity));

        if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>()) {
            auto&& playerPawn = baseEntity.template as<PlayerPawn>();
            playerPawn.weapons().forEach([this](const auto& weaponEntity) {
                const auto entityClass = weaponEntity.entityIdentity().entityClass();
                hookContext.entityClassifier().registerWeaponClass(entityClass);
            });
            hookContext.template make<PlayerInfoInWorld>().drawPlayerInformation(playerPawn);
            updateModelGlow<PlayerModelGlow>(playerPawn, entityTypeInfo);
            applyOutlineGlow<PlayerOutlineGlow>(playerPawn, entityTypeInfo);
            if (bombPlantAlertVisibility != Visibility::Visible)
                bombPlantAlertVisibility = hookContext.template make<BombPlantAlert>().show(playerPawn);
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
        } else if (entityTypeInfo.template is<cs2::C_C4>()) {
            auto&& weapon = baseEntity.template as<BaseWeapon>();
            updateModelGlow<DroppedBombModelGlow>(weapon, entityTypeInfo);
            applyOutlineGlow<DroppedBombOutlineGlow>(baseEntity, entityTypeInfo);
        } else if (entityTypeInfo.isWeapon()) {
            auto&& weapon = baseEntity.template as<BaseWeapon>();
            if (weapon.isC4()) {
                updateModelGlow<DroppedBombModelGlow>(weapon, entityTypeInfo);
                applyOutlineGlow<DroppedBombOutlineGlow>(baseEntity, entityTypeInfo);
            } else {
                updateModelGlow<WeaponModelGlow>(weapon, entityTypeInfo);
                applyOutlineGlow<WeaponOutlineGlow>(weapon, entityTypeInfo);
            }
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
