#pragma once

#include <GameClasses/Hud/Hud.h>
#include <GameClasses/PanelHandle.h>
#include <GameClasses/PanoramaUiPanel.h>

struct InWorldPanelContainerState {
    cs2::PanelHandle containerPanelHandle;
};

template <typename HookContext>
struct InWorldPanelContainerUnloadHandler {
    explicit InWorldPanelContainerUnloadHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void handleUnload() const noexcept
    {
        hookContext.template make<PanoramaUiEngine>().deletePanelByHandle(hookContext.inWorldPanelContainerState().containerPanelHandle);
    }

private:
    HookContext& hookContext;
};

template <typename HookContext>
class InWorldPanelContainer {
public:
    explicit InWorldPanelContainer(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto get() noexcept
    {
        return handle().getOrInit(createContainerPanel());
    }

private:
    [[nodiscard]] decltype(auto) handle() const noexcept
    {
        return hookContext.template make<PanelHandle>(hookContext.inWorldPanelContainerState().containerPanelHandle);
    }

    [[nodiscard]] auto createContainerPanel() const noexcept
    {
        return [this] {
            auto&& panel = hookContext.panelFactory().createPanel(hookContext.hud().getHudReticle()).uiPanel();
            panel.fitParent();
            return panel;
        };
    }

    HookContext& hookContext;
};
