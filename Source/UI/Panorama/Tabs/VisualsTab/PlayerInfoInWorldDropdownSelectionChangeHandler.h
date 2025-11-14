#pragma once

#include <optional>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct PlayerInfoInWorldDropdownSelectionChangeHandler {
    explicit PlayerInfoInWorldDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        bool enabled;
        std::optional<bool> onlyEnemies;

        switch (selectedIndex) {
        case 0: enabled = true; onlyEnemies = true; break;
        case 1: enabled = true; onlyEnemies = false; break;
        case 2: enabled = false; break;
        default: return;
        }

        SET_CONFIG_VAR(player_info_vars::Enabled, enabled);
        if (onlyEnemies.has_value())
            SET_CONFIG_VAR(player_info_vars::OnlyEnemies, *onlyEnemies);
    }

private:
    HookContext& hookContext;
};
