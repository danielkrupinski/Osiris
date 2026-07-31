#pragma once

#include <cstdint>
#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct AimbotLockModeSelectionChangeHandler {
    explicit AimbotLockModeSelectionChangeHandler(HookContext& hc) noexcept : hookContext{hc} {}
    void onSelectionChanged(int idx) {
        if (idx < 0 || idx > 1) return;
        SET_CONFIG_VAR(aimbot_vars::LockMode, static_cast<AimbotLockMode>(idx));
    }
private:
    HookContext& hookContext;
};
