#pragma once

#include "DefusingAlertState.h"

template <typename HookContext>
struct DefusingAlertUnloadHandler {
    DefusingAlertUnloadHandler(HookContext& hookContext, const DefusingAlertState& state) noexcept
        : hookContext{hookContext}
        , state{state}
    {
    }

    void handleUnload() const noexcept
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(state.defusingAlertContainerPanelHandle);
    }

private:
    HookContext& hookContext;
    const DefusingAlertState& state;
};
