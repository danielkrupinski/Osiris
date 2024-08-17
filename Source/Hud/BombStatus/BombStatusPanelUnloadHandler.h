#pragma once

#include "BombStatusPanel.h"

template <typename HookContext>
struct BombStatusPanelUnloadHandler {
    explicit BombStatusPanelUnloadHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void handleUnload() const noexcept
    {
        hookContext.template make<BombStatusPanel>().restore();
        hookContext.panels().deletePanelByHandle(hookContext.bombStatusPanelState().invisiblePanelHandle);
    }

private:
    HookContext& hookContext;
};
