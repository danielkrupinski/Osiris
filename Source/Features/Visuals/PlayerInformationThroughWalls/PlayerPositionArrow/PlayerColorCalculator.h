#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <CS2/Constants/ColorConstants.h>

template <typename PlayerColorIndexAccessor>
struct PlayerColorCalculator {
    PlayerColorIndexAccessor playerColorIndexAccessor;

    [[nodiscard]] bool getPlayerColor(cs2::Color* color) const noexcept
    {
        return getPlayerColor(playerColorIndexAccessor.getPlayerColorIndex(), color);
    }

private:
    [[nodiscard]] bool getPlayerColor(cs2::CCSPlayerController::m_iCompTeammateColor playerColorIndex, cs2::Color* color) const noexcept
    {
        if (playerColorIndex >= 0 && std::cmp_less(playerColorIndex, cs2::kPlayerColors.size())) {
            *color = cs2::kPlayerColors[playerColorIndex];
            return true;
        }
        return false;
    }
};

template <typename PlayerColorIndexAccessor>
PlayerColorCalculator(PlayerColorIndexAccessor) -> PlayerColorCalculator<PlayerColorIndexAccessor>;
