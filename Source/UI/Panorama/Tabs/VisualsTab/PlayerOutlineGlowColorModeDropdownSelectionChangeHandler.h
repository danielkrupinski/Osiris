#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct PlayerOutlineGlowColorModeDropdownSelectionChangeHandler {
    explicit PlayerOutlineGlowColorModeDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        PlayerOutlineGlowColorType colorMode;

        switch (selectedIndex) {
        case 0: colorMode = PlayerOutlineGlowColorType::PlayerOrTeamColor; break;
        case 1: colorMode = PlayerOutlineGlowColorType::TeamColor; break;
        case 2: colorMode = PlayerOutlineGlowColorType::HealthBased; break;
        case 3: colorMode = PlayerOutlineGlowColorType::EnemyAlly; break;
        default: return;
        }

        SET_CONFIG_VAR(outline_glow_vars::PlayerGlowColorMode, colorMode);
    }

private:
    HookContext& hookContext;
};
