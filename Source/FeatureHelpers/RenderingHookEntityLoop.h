#pragma once

#include <CS2/Classes/Entities/CEntityInstance.h>
#include <Features/Visuals/ModelGlow/ModelGlow.h>
#include <Features/Visuals/OutlineGlow/OutlineGlow.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorld.h>

template <typename HookContext>
class RenderingHookEntityLoop {
public:
    explicit RenderingHookEntityLoop(HookContext& hookContext, PlayerInfoInWorld<HookContext>& playerInformationThroughWalls) noexcept
        : hookContext{hookContext}
        , playerInformationThroughWalls{playerInformationThroughWalls}
    {
    }

    void run() const noexcept
    {
        hookContext.template make<EntitySystem>().iterateEntities([this](auto& entity) { handleEntity(entity); });
        hookContext.template make<ModelGlow>().onEntityListTraversed();
    }

private:
    void handleEntity(cs2::CEntityInstance& entity) const noexcept
    {
        auto&& baseEntity = hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(&entity));

        const auto entityTypeInfo = hookContext.entityClassifier().classifyEntity(hookContext.gameDependencies().entitiesVMTs, entity.vmt);

        if (entityTypeInfo.template is<cs2::C_CSPlayerPawn>()) {
            auto&& playerPawn = hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(&entity));
            playerInformationThroughWalls.drawPlayerInformation(playerPawn);
            hookContext.template make<ModelGlow>().updateSceneObjectUpdaterHook(playerPawn);
        }

        if (entityTypeInfo.isModelEntity())
            hookContext.template make<OutlineGlow>().applyGlowToEntity(entityTypeInfo, baseEntity.template as<BaseModelEntity>());
    }

    HookContext& hookContext;
    PlayerInfoInWorld<HookContext>& playerInformationThroughWalls;
};
