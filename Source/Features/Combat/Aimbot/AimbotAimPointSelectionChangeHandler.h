#pragma once

#include <cstdint>

#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct AimbotAimPointSelectionChangeHandler {
    explicit AimbotAimPointSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        if (selectedIndex < 0 || selectedIndex > 4)
            return;
        SET_CONFIG_VAR(aimbot_vars::AimPoint, static_cast<AimbotAimPoint>(selectedIndex));
    }

private:
    HookContext& hookContext;
};
