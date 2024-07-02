#pragma once

#include <CS2/Classes/Entities/CEntityInstance.h>
#include <Features/Visuals/PlayerInformationThroughWalls/PlayerInformationThroughWalls.h>

#include "EntityFromHandleFinder.h"

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
        if (!dependencies.requestDependencies(kCrucialDependencies))
            return;

        dependencies.getDependency<EntityListWalker>().iterateEntities([this](auto& entity) { handleEntity(entity); });
    }

private:
    static constexpr auto kCrucialDependencies{
        HookDependenciesMask{}
        .set<EntityListWalker>()
    };

    void handleEntity(cs2::CEntityInstance& entity) const noexcept
    {
        if (dependencies.gameDependencies().entitiesVMTs.isPlayerPawn(entity.vmt)) {
            auto& playerPawn = static_cast<cs2::C_CSPlayerPawn&>(entity);
            playerInformationThroughWalls.drawPlayerInformation(playerPawn);
        }
    }

    HookDependencies& dependencies;
    PlayerInformationThroughWalls& playerInformationThroughWalls;
};
