#pragma once

#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct PlayerInfoInWorldPlayerHealthColorModeDropdownSelectionChangeHandler {
    explicit PlayerInfoInWorldPlayerHealthColorModeDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        PlayerHealthTextColor colorMode;
        switch (selectedIndex) {
        case 0: colorMode = PlayerHealthTextColor::HealthBased; break;
        case 1: colorMode = PlayerHealthTextColor::White; break;
        default: return;
        }

        SET_CONFIG_VAR(player_info_vars::PlayerHealthColorMode, colorMode);
    }

private:
    HookContext& hookContext;
};
