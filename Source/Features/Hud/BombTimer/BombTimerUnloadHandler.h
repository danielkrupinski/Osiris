#pragma once

#include "BombTimerState.h"
#include <GameClient/Panorama/PanoramaUiEngine.h>

template <typename HookContext>
struct BombTimerUnloadHandler {
    BombTimerUnloadHandler(HookContext& hookContext, const BombTimerState& state) noexcept
        : hookContext{hookContext}
        , state{state}
    {
    }

    void handleUnload() const noexcept
    {
        auto&& uiEngine = hookContext.template make<PanoramaUiEngine>();
        uiEngine.deletePanelByHandle(state.bombTimerPanelHandle);
        uiEngine.deletePanelByHandle(state.bombTimerContainerPanelHandle);
    }

    HookContext& hookContext;
    const BombTimerState& state;
};
