#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <FeatureHelpers/TeamNumber.h>
#include <GameClasses/EntitySystem.h>
#include "PlayerPawn.h"

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
        const auto playerPawnHandle = hookContext.clientPatternSearchResults().template get<OffsetToPlayerPawnHandle>().of(playerControllerPointer).get();
        if (!playerPawnHandle)
            return hookContext.template make<PlayerPawn>(nullptr);
        return hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(hookContext.template make<EntitySystem>().getEntityFromHandle(*playerPawnHandle)));
    }

    [[nodiscard]] std::optional<cs2::Color> getPlayerColor() const noexcept
    {
        return getPlayerColor(cs2::kPlayerColors);
    }

    [[nodiscard]] std::optional<cs2::Color> getPlayerColorSaturated() const noexcept
    {
        return getPlayerColor(cs2::kPlayerColorsSaturated);
    }

    [[nodiscard]] std::optional<cs2::Color> getPlayerColorHalfSaturated() const noexcept
    {
        return getPlayerColor(cs2::kPlayerColorsHalfSaturated);
    }

private:
    [[nodiscard]] std::optional<cs2::Color> getPlayerColor(std::span<const cs2::Color> playerColors) const noexcept
    {
        const auto playerColorIndex = hookContext.clientPatternSearchResults().template get<OffsetToPlayerColor>().of(playerControllerPointer).get();
        if (playerColorIndex && *playerColorIndex >= 0 && std::cmp_less(*playerColorIndex, playerColors.size()))
            return playerColors[*playerColorIndex];
        return {};
    }

    HookContext& hookContext;
    cs2::CCSPlayerController* playerControllerPointer;
};
