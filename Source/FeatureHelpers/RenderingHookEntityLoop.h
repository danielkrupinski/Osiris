#pragma once

#include <CS2/Classes/Entities/CEntityInstance.h>
#include <Features/Visuals/PlayerInformationThroughWalls/PlayerInformationThroughWalls.h>
#include <Features/Visuals/PlayerOutlineGlow/PlayerOutlineGlow.h>

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
        if (dependencies.gameDependencies().entitiesVMTs.isPlayerPawn(entity.vmt)) {
            auto&& playerPawn = dependencies.make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(&entity));
            playerInformationThroughWalls.drawPlayerInformation(playerPawn);
            dependencies.make<PlayerOutlineGlow>().applyGlowToPlayer(playerPawn);
        }
    }

    HookDependencies& dependencies;
    PlayerInformationThroughWalls& playerInformationThroughWalls;
};
