#pragma once

#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct PlayerInfoInWorldPlayerNameColorModeDropdownSelectionChangeHandler {
    explicit PlayerInfoInWorldPlayerNameColorModeDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        PlayerNameColorType colorMode;
        switch (selectedIndex) {
        case 0: colorMode = PlayerNameColorType::White; break;
        case 1: colorMode = PlayerNameColorType::TeamColor; break;
        case 2: colorMode = PlayerNameColorType::HealthBased; break;
        default: return;
        }

        SET_CONFIG_VAR(player_info_vars::PlayerNameColorMode, colorMode);
    }

private:
    HookContext& hookContext;
};
