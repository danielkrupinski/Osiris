#pragma once

#include <CS2/Classes/Entities/CEntityInstance.h>
#include <Features/Visuals/OutlineGlow/OutlineGlow.h>
#include <Features/Visuals/PlayerInformationThroughWalls/PlayerInformationThroughWalls.h>

#include <HookDependencies/HookDependenciesMask.h>

class RenderingHookEntityLoop {
public:
    explicit RenderingHookEntityLoop(HookDependencies& dependencies, PlayerInformationThroughWalls& playerInformationThroughWalls) noexcept
        : dependencies{dependencies}
        , playerInformationThroughWalls{playerInformationThroughWalls}
    {
    }

    void run() const noexcept
    {
        dependencies.make<EntitySystem>().iterateEntities([this](auto& entity) { handleEntity(entity); });
    }

private:
    void handleEntity(cs2::CEntityInstance& entity) const noexcept
    {
        const auto entityTypeInfo = dependencies.entityClassifier().classifyEntity(dependencies.gameDependencies().entitiesVMTs, entity.vmt);

        if (entityTypeInfo.typeIndex == utils::typeIndex<cs2::C_CSPlayerPawn, KnownEntityTypes>()) {
            auto&& playerPawn = dependencies.make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(&entity));
            playerInformationThroughWalls.drawPlayerInformation(playerPawn);
        }
        dependencies.make<OutlineGlow>().applyGlowToEntity(entityTypeInfo, entity);
    }

    HookDependencies& dependencies;
    PlayerInformationThroughWalls& playerInformationThroughWalls;
};
