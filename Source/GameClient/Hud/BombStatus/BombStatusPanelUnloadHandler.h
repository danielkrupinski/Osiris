#pragma once

#include "BombStatusPanel.h"
#include <GameClient/Panorama/PanoramaUiEngine.h>

template <typename HookContext>
struct BombStatusPanelUnloadHandler {
    explicit BombStatusPanelUnloadHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void handleUnload() const noexcept
    {
        hookContext.template make<BombStatusPanel>().restore();
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(hookContext.bombStatusPanelState().invisiblePanelHandle);
    }

private:
    HookContext& hookContext;
};
