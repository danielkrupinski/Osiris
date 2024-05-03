#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>

#include "PlayerPositionArrowColorType.h"

template <typename PlayerColorCalculator, typename TeamColorCalculator>
struct PlayerPositionArrowColorCalculator {
    [[nodiscard]] cs2::Color getArrowColor(PlayerPositionArrowColorType colorType) const noexcept
    {
        if (cs2::Color color{cs2::kColorWhite}; colorType == PlayerPositionArrowColorType::PlayerOrTeamColor && playerColorCalculator.getPlayerColor(&color))
            return color;
        return teamColorCalculator.getTeamColor();
    }

    PlayerColorCalculator playerColorCalculator;
    TeamColorCalculator teamColorCalculator;
};

template <typename PlayerColorCalculator, typename TeamColorCalculator>
PlayerPositionArrowColorCalculator(PlayerColorCalculator, TeamColorCalculator) ->
    PlayerPositionArrowColorCalculator<PlayerColorCalculator, TeamColorCalculator>;
