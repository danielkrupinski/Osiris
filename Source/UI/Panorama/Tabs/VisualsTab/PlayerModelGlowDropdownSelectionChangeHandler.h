#pragma once

#include <optional>
#include <Features/Visuals/ModelGlow/ModelGlowConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct PlayerModelGlowDropdownSelectionChangeHandler {
    explicit PlayerModelGlowDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        bool glowPlayers;
        std::optional<bool> glowOnlyEnemies;

        switch (selectedIndex) {
        case 0: glowPlayers = true; glowOnlyEnemies = true; break;
        case 1: glowPlayers = true; glowOnlyEnemies = false; break;
        case 2: glowPlayers = false; break;
        default: return;
        }

        SET_CONFIG_VAR(model_glow_vars::GlowPlayers, glowPlayers);
        if (glowOnlyEnemies.has_value())
            SET_CONFIG_VAR(model_glow_vars::GlowOnlyEnemies, *glowOnlyEnemies);
    }

private:
    HookContext& hookContext;
};
