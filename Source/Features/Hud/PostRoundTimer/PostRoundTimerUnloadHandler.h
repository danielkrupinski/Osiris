#pragma once

#include "PostRoundTimerState.h"

template <typename HookContext>
struct PostRoundTimerUnloadHandler {
    PostRoundTimerUnloadHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void handleUnload() const noexcept
    {
        hookContext.panels().deletePanelByHandle(state().countdownContainerPanelHandle);
    }

private:
    [[nodiscard]] const auto& state() const noexcept
    {
        return hookContext.featuresStates().hudFeaturesStates.postRoundTimerState;
    }

    HookContext& hookContext;
};
