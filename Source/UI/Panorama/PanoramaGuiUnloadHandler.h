#pragma once

#include <GameClient/Panorama/PanoramaUiEngine.h>

template <typename HookContext>
struct PanoramaGuiUnloadHandler {
    explicit PanoramaGuiUnloadHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void handleUnload() const noexcept
    {
        auto&& uiEngine = hookContext.template make<PanoramaUiEngine>();
        uiEngine.deletePanelByHandle(state().guiButtonHandle);
        uiEngine.deletePanelByHandle(state().guiPanelHandle);

        if (auto&& settingsPanel = uiEngine.getPanelFromHandle(state().settingsPanelHandle))
            uiEngine.runScript(settingsPanel, "delete $.Osiris");
    }

private:
    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.panoramaGuiState();
    }

    HookContext& hookContext;
};
