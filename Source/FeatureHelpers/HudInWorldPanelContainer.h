#pragma once

#include <GameClasses/Hud/Hud.h>
#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <Helpers/PanoramaPanelPointer.h>

class HudInWorldPanelContainer {
public:
    HudInWorldPanelContainer() = default;
    HudInWorldPanelContainer(const HudInWorldPanelContainer&) = delete;
    HudInWorldPanelContainer(HudInWorldPanelContainer&&) = delete;
    HudInWorldPanelContainer& operator=(const HudInWorldPanelContainer&) = delete;
    HudInWorldPanelContainer& operator=(HudInWorldPanelContainer&&) = delete;

    ~HudInWorldPanelContainer() noexcept
    {
        if (containerPanel.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(containerPanel.getHandle());
    }

    template <typename Dependencies>
    [[nodiscard]] auto get(Hud<Dependencies> hud, auto& hookContext) noexcept
    {
        if (const auto container = containerPanel.get())
            return PanoramaUiPanel{PanoramaUiPanelContext{hookContext, container}};
        return createPanel(hud, hookContext);
    }

private:
    template <typename Dependencies>
    [[nodiscard]] auto createPanel(Hud<Dependencies> hud, auto& hookContext) noexcept
    {
        if (const auto hudReticle = hud.getHudReticle()) {
            if (const auto clientPanel = Panel::create("", hudReticle)) {
                PanoramaUiPanel panel{PanoramaUiPanelContext{hookContext, clientPanel->uiPanel}};
                panel.fitParent();
                containerPanel = clientPanel->uiPanel;
                return panel;
            }
        }
        return PanoramaUiPanel{PanoramaUiPanelContext{hookContext, nullptr}};
    }

    PanoramaPanelPointer containerPanel;
};
