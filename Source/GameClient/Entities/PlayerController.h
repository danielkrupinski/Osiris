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

    [[nodiscard]] decltype(auto) pawn() const noexcept
    {
        const auto playerPawnHandle = hookContext.patternSearchResults().template get<OffsetToBasePawnHandle>().of(playerControllerPointer).get();
        if (!playerPawnHandle)
            return hookContext.template make<BaseEntity>(nullptr);
        return hookContext.template make<EntitySystem>().getEntityFromHandle2(*playerPawnHandle);
    }

    [[nodiscard]] decltype(auto) playerColorIndex() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToPlayerColor>().of(playerControllerPointer).toOptional();
    }

private:
    HookContext& hookContext;
    cs2::CCSPlayerController* playerControllerPointer;
};
