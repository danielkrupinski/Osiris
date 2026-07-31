#pragma once

#include <cstdint>

#include <Features/Combat/Aimbot/AimbotConfigVariables.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct AimbotMovementPenaltySelectionChangeHandler {
    explicit AimbotMovementPenaltySelectionChangeHandler(HookContext& hc) noexcept : hookContext{hc} {}

    void onSelectionChanged(int idx) {
        if (idx < 0 || idx > 2) return;
        SET_CONFIG_VAR(aimbot_vars::MovementPenalty, static_cast<AimbotMovementPenalty>(idx));
    }

private:
    HookContext& hookContext;
};
