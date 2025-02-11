#pragma once

#include <optional>
#include <span>
#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/BaseEntity.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <MemoryPatterns/PatternTypes/PlayerControllerPatternTypes.h>
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

    [[nodiscard]] decltype(auto) playerColorIndex() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToPlayerColor>().of(playerControllerPointer).toOptional();
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
