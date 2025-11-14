#pragma once

#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct PlayerInfoInWorldPlayerPositionArrowColorModeDropdownSelectionChangeHandler {
    explicit PlayerInfoInWorldPlayerPositionArrowColorModeDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        PlayerPositionArrowColorType colorMode;
        switch (selectedIndex) {
        case 0: colorMode = PlayerPositionArrowColorType::PlayerOrTeamColor; break;
        case 1: colorMode = PlayerPositionArrowColorType::TeamColor; break;
        default: return;
        }

        SET_CONFIG_VAR(player_info_vars::PlayerPositionArrowColorMode, colorMode);
    }

private:
    HookContext& hookContext;
};
