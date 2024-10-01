#pragma once

#include <CS2/Classes/Entities/CEntityInstance.h>
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
    }

private:
    void handleEntity(cs2::CEntityInstance& entity) const noexcept
    {
        const auto entityTypeInfo = hookContext.entityClassifier().classifyEntity(hookContext.gameDependencies().entitiesVMTs, entity.vmt);

        if (entityTypeInfo.typeIndex == utils::typeIndex<cs2::C_CSPlayerPawn, KnownEntityTypes>()) {
            auto&& playerPawn = hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(&entity));
            playerInformationThroughWalls.drawPlayerInformation(playerPawn);
        }
        hookContext.template make<OutlineGlow>().applyGlowToEntity(entityTypeInfo, entity);
    }

    HookContext& hookContext;
    PlayerInfoInWorld<HookContext>& playerInformationThroughWalls;
};
