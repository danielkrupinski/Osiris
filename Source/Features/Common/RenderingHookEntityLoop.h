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
#include <Features/Combat/Aimbot/Aimbot.h>
#include <Features/Combat/BHop/BHop.h>
#include <Features/Combat/ThirdPerson/ThirdPerson.h>
#include <Features/Combat/BoneDumper/BoneDumper.h>
#include <Features/Combat/Triggerbot/Triggerbot.h>
#include <Features/Hud/BombPlantAlert/BombPlantAlert.h>
#include <Features/Hud/SpectatorList/SpectatorList.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <GameClient/Entities/PlayerController.h>

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
        auto spectatorList = hookContext.template make<SpectatorList>();
        auto triggerbot = hookContext.template make<Triggerbot>();
        auto aimbot = hookContext.template make<Aimbot>();
        auto boneDumper = hookContext.template make<BoneDumper>();
        boneDumper.beginFrame();
        hookContext.template make<EntitySystem>().forEachNetworkableEntityIdentity([this, &bombPlantAlertVisibility, &spectatorList, &triggerbot, &aimbot, &boneDumper](const auto& entityIdentity) {
            handleEntityIdentity(entityIdentity, bombPlantAlertVisibility, spectatorList, triggerbot, aimbot, boneDumper);
        });
        boneDumper.endFrame();
        hookContext.template make<ModelGlow>().postUpdateInMainThread();
        if (bombPlantAlertVisibility == Visibility::Hidden)
            hookContext.template make<BombPlantAlert>().hide();
        spectatorList.render();
        hookContext.template make<BHop>().run();
        hookContext.template make<ThirdPerson>().run();
        aimbot.execute();
        triggerbot.execute();
    }

private:
    void handleEntityIdentity(const cs2::CEntityIdentity& entityIdentity, Visibility& bombPlantAlertVisibility, SpectatorList<HookContext>& spectatorList, Triggerbot<HookContext>& triggerbot, Aimbot<HookContext>& aimbot, BoneDumper<HookContext>& boneDumper) const noexcept
    {
        const auto entityTypeInfo = hookContext.entityClassifier().classifyEntity(entityIdentity.entityClass);
        auto&& baseEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(entityIdentity.entity));

        if (entityTypeInfo.template is<cs2::CCSPlayerController>()) {
            spectatorList.addPotentialSpectator(hookContext.template make<PlayerController>(static_cast<cs2::CCSPlayerController*>(entityIdentity.entity)));
        } else if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>()) {
            auto&& playerPawn = baseEntity.template as<PlayerPawn>();
            hookContext.template make<PlayerInfoInWorld>().drawPlayerInformation(playerPawn);
            updateModelGlow<PlayerModelGlow>(playerPawn, entityTypeInfo);
            applyOutlineGlow<PlayerOutlineGlow>(playerPawn, entityTypeInfo);
            triggerbot.checkPlayer(playerPawn);
            aimbot.checkPlayer(playerPawn);
            boneDumper.addEnemy(playerPawn);
            if (bombPlantAlertVisibility != Visibility::Visible)
                bombPlantAlertVisibility = hookContext.template make<BombPlantAlert>().show(playerPawn);
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
