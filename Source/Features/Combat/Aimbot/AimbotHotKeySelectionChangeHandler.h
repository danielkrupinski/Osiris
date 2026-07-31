#pragma once

#include <cstdint>

#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct AimbotHotKeySelectionChangeHandler {
    explicit AimbotHotKeySelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        if (selectedIndex < 0 || selectedIndex > 4)
            return;
        SET_CONFIG_VAR(aimbot_vars::HotKey, static_cast<AimbotKey>(selectedIndex));
    }

private:
    HookContext& hookContext;
};
