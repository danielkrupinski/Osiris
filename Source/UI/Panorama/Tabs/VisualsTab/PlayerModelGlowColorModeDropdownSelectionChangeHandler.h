#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct PlayerModelGlowColorModeDropdownSelectionChangeHandler {
    explicit PlayerModelGlowColorModeDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        PlayerModelGlowColorType colorMode;
        switch (selectedIndex) {
        case 0: colorMode = PlayerModelGlowColorType::PlayerOrTeamColor; break;
        case 1: colorMode = PlayerModelGlowColorType::TeamColor; break;
        case 2: colorMode = PlayerModelGlowColorType::HealthBased; break;
        case 3: colorMode = PlayerModelGlowColorType::EnemyAlly; break;
        default: return;
        }

        SET_CONFIG_VAR(model_glow_vars::PlayerGlowColorMode, colorMode);
    }

private:
    HookContext& hookContext;
};
