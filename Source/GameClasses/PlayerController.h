#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <FeatureHelpers/TeamNumber.h>

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

private:
    [[nodiscard]] const auto& entityDeps() const noexcept
    {
        return hookContext.gameDependencies().entityDeps;
    }

    HookContext& hookContext;
    cs2::CCSPlayerController* playerControllerPointer;
};
