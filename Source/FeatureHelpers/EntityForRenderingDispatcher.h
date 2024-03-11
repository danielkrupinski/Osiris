#pragma once

#include <CS2/Classes/Entities/CEntityInstance.h>
#include <Features/Visuals/PlayerPositionThroughWalls.h>

#include "EntityFromHandleFinder.h"

#include <HookDependencies/HookDependenciesMask.h>

struct RenderingHookEntityLoopDependencies {
    [[nodiscard]] HookDependenciesMask getRenderingHookDependencies() const noexcept
    {
        HookDependenciesMask dependencies;
        if (playerPositionThroughWallsState.enabled) {
            dependencies.set<LocalPlayerController>()
                .set<OffsetToPlayerPawnHandle>()
                .set<EntityFromHandleFinder>()
                .set<EntitiesVMTs>()
                .set<EntityListWalker>()
                .set<OffsetToHealth>();
        }

        return dependencies;
    }

    const PlayerPositionThroughWallsState& playerPositionThroughWallsState;
};

class RenderingHookEntityLoop {
public:
    explicit RenderingHookEntityLoop(const HookDependencies& dependencies, PlayerPositionThroughWalls& playerPositionThroughWalls) noexcept
        : dependencies{dependencies}
        , playerPositionThroughWalls{playerPositionThroughWalls}
    {
    }

    void run() const noexcept
    {
        if (!dependencies.hasDependencies(kCrucialDependencies))
            return;

        dependencies.getDependency<EntityListWalker>().iterateEntities([this](auto& entity) { handleEntity(entity); });
    }

private:
    static constexpr auto kCrucialDependencies{
        HookDependenciesMask{}
        .set<OffsetToPlayerPawnHandle>()
        .set<EntityFromHandleFinder>()
        .set<EntitiesVMTs>()
        .set<EntityListWalker>()
    };

    void handleEntity(cs2::CEntityInstance& entity) const noexcept
    {
        if (!dependencies.hasDependencies(kCrucialDependencies))
            return;

        if (dependencies.getDependency<EntitiesVMTs>().isPlayerPawn(entity.vmt)) {
            auto& playerPawn = static_cast<cs2::C_CSPlayerPawn&>(entity);
            if (isAlive(playerPawn))
                playerPositionThroughWalls.run(playerPawn);
        }
    }

    [[nodiscard]] bool isAlive(cs2::C_BaseEntity& entity) const noexcept
    {
        if (dependencies.hasDependency<OffsetToHealth>())
            return *dependencies.getDependency<OffsetToHealth>().of(&entity).get() > 0;
        return true;
    }

    [[nodiscard]] bool isLocalPlayerController(cs2::CCSPlayerController& playerController) const noexcept
    {
        return dependencies.hasDependency<LocalPlayerController>() && dependencies.getDependency<LocalPlayerController>() == &playerController;
    }

    const HookDependencies& dependencies;
    PlayerPositionThroughWalls& playerPositionThroughWalls;
};
