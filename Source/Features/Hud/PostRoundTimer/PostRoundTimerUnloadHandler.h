#pragma once

#include <GameClient/Panorama/PanoramaUiEngine.h>

template <typename HookContext>
struct PostRoundTimerUnloadHandler {
    PostRoundTimerUnloadHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void handleUnload() const noexcept
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(state().countdownContainerPanelHandle);
    }

private:
    [[nodiscard]] const auto& state() const noexcept
    {
        return hookContext.featuresStates().hudFeaturesStates.postRoundTimerState;
    }

    HookContext& hookContext;
};
