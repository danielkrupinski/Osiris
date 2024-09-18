#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <FeatureHelpers/TeamNumber.h>
#include "PlayerPawn.h"
#include "EntitySystem.h"

template <typename HookContext>
class PlayerController {
public:
    PlayerController(HookContext& hookContext, cs2::CCSPlayerController* playerControllerPointer) noexcept
        : hookContext{hookContext}
        , playerControllerPointer{playerControllerPointer}
    {
    }

    [[nodiscard]] decltype(auto) baseEntity() const noexcept
    {
        return hookContext.template make<BaseEntity>(playerControllerPointer);
    }

    [[nodiscard]] bool operator==(const PlayerController& other) const noexcept
    {
        return playerControllerPointer != nullptr && playerControllerPointer == other.playerControllerPointer;
    }

    [[nodiscard]] TeamNumber teamNumber() const noexcept
    {
        return baseEntity().teamNumber();
    }

    [[nodiscard]] decltype(auto) playerPawn() const noexcept
    {
        const auto playerPawnHandle = hookContext.gameDependencies().playerControllerDeps.offsetToPlayerPawnHandle.of(playerControllerPointer).get();
        if (!playerPawnHandle)
            return hookContext.template make<PlayerPawn>(nullptr);
        return hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(hookContext.template make<EntitySystem>().getEntityFromHandle(*playerPawnHandle)));
    }

    [[nodiscard]] std::optional<cs2::Color> getPlayerColor() const noexcept
    {
        const auto playerColorIndex = hookContext.gameDependencies().playerControllerDeps.offsetToPlayerColor.of(playerControllerPointer).get();
        if (playerColorIndex && *playerColorIndex >= 0 && std::cmp_less(*playerColorIndex, cs2::kPlayerColors.size()))
            return cs2::kPlayerColors[*playerColorIndex];
        return {};
    }

private:
    HookContext& hookContext;
    cs2::CCSPlayerController* playerControllerPointer;
};
