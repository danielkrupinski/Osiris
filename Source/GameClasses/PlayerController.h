#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <FeatureHelpers/TeamNumber.h>
#include "PlayerPawn.h"

template <typename HookContext>
class PlayerController {
public:
    PlayerController(HookContext& hookContext, cs2::CCSPlayerController* playerControllerPointer) noexcept
        : hookContext{hookContext}
        , playerControllerPointer{playerControllerPointer}
    {
    }

    [[nodiscard]] TeamNumber teamNumber() const noexcept
    {
        return TeamNumber{entityDeps().offsetToTeamNumber.of(playerControllerPointer).valueOr({})};
    }

    [[nodiscard]] decltype(auto) playerPawn() const noexcept
    {
        if (!hookContext.template requestDependency<EntityFromHandleFinder>())
            return hookContext.template make<PlayerPawn>(nullptr);

        const auto playerPawnHandle = hookContext.gameDependencies().playerControllerDeps.offsetToPlayerPawnHandle.of(playerControllerPointer).get();
        if (!playerPawnHandle)
            return hookContext.template make<PlayerPawn>(nullptr);
        return hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(hookContext.template getDependency<EntityFromHandleFinder>().getEntityFromHandle(*playerPawnHandle)));
    }

private:
    [[nodiscard]] const auto& entityDeps() const noexcept
    {
        return hookContext.gameDependencies().entityDeps;
    }

    HookContext& hookContext;
    cs2::CCSPlayerController* playerControllerPointer;
};
