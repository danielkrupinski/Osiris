#pragma once

#include <CS2/Classes/Entities/CEntityInstance.h>
#include <Features/Visuals/PlayerPositionThroughWalls.h>

#include "EntityFromHandleFinder.h"

#include <HookDependencies/HookDependenciesMask.h>

class RenderingHookEntityLoop {
public:
    explicit RenderingHookEntityLoop(HookDependencies& dependencies, PlayerPositionThroughWalls& playerPositionThroughWalls) noexcept
        : dependencies{dependencies}
        , playerPositionThroughWalls{playerPositionThroughWalls}
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
        .set<EntitiesVMTs>()
        .set<EntityListWalker>()
    };

    void handleEntity(cs2::CEntityInstance& entity) const noexcept
    {
        if (dependencies.getDependency<EntitiesVMTs>().isPlayerPawn(entity.vmt)) {
            auto& playerPawn = static_cast<cs2::C_CSPlayerPawn&>(entity);
            if (isAlive(playerPawn))
                playerPositionThroughWalls.run(playerPawn);
        }
    }

    [[nodiscard]] bool isAlive(cs2::C_BaseEntity& entity) const noexcept
    {
        if (dependencies.requestDependency<OffsetToHealth>())
            return *dependencies.getDependency<OffsetToHealth>().of(&entity).get() > 0;
        return true;
    }

    HookDependencies& dependencies;
    PlayerPositionThroughWalls& playerPositionThroughWalls;
};
