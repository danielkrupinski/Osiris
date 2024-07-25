#pragma once

#include "BombTimerState.h"

template <typename HookContext>
struct BombTimerUnloadHandler {
    BombTimerUnloadHandler(HookContext& hookContext, const BombTimerState& state) noexcept
        : hookContext{hookContext}
        , state{state}
    {
    }

    void handleUnload() const noexcept
    {
        auto&& hud = hookContext.hud();
        hud.bombStatus().setParent(hud.scoreAndTimeAndBomb());

        auto&& panels = hookContext.panels();
        panels.deletePanelByHandle(state.invisiblePanelHandle);
        panels.deletePanelByHandle(state.bombTimerPanelHandle);
        panels.deletePanelByHandle(state.bombTimerContainerPanelHandle);
    }

    HookContext& hookContext;
    const BombTimerState& state;
};
