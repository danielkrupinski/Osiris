#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>

#include "PlayerPositionArrowColorType.h"

template <typename TeamColorCalculator>
struct PlayerPositionArrowColorCalculator {
    [[nodiscard]] cs2::Color getArrowColor(auto&& playerController, PlayerPositionArrowColorType colorType) const noexcept
    {
        if (colorType == PlayerPositionArrowColorType::PlayerOrTeamColor) {
            const auto playerColor = playerController.getPlayerColor();
            if (playerColor.has_value())
                return *playerColor;
        }
        return teamColorCalculator.getTeamColor();
    }

    TeamColorCalculator teamColorCalculator;
};

template <typename TeamColorCalculator>
PlayerPositionArrowColorCalculator(TeamColorCalculator) ->
    PlayerPositionArrowColorCalculator<TeamColorCalculator>;
