#pragma once

#include <HookContext/HookContextMacros.h>

template <typename HookContext, typename ConfigVariable>
struct OnOffDropdownSelectionChangeHandler {
    explicit OnOffDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        bool isOn;
        switch (selectedIndex) {
        case 0: isOn = true; break;
        case 1: isOn = false; break;
        default: return;
        }
        SET_CONFIG_VAR(ConfigVariable, isOn);
    }

private:
    HookContext& hookContext;
};
