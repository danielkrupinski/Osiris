#pragma once

#include <GameClient/Panorama/PanoramaUiEngine.h>

template <typename HookContext>
struct InWorldPanelsUnloadHandler {
    explicit InWorldPanelsUnloadHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void handleUnload() const noexcept
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(hookContext.inWorldPanelsState().containerPanelHandle);
    }

private:
    HookContext& hookContext;
};
