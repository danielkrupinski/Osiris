#pragma once

#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>

struct PlayerColorIndexAccessor {
    [[nodiscard]] cs2::CCSPlayerController::m_iCompTeammateColor getPlayerColorIndex() const noexcept
    {
        constexpr auto kInvalidPlayerColorIndex = cs2::CCSPlayerController::m_iCompTeammateColor{-1};
        return offsetToPlayerColor.of(&playerController).valueOr(kInvalidPlayerColorIndex);
    }

    cs2::CCSPlayerController& playerController;
    OffsetToPlayerColor offsetToPlayerColor;
};
