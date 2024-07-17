#pragma once

#include <GameClasses/Hud/Hud.h>
#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <Helpers/PanoramaPanelPointer.h>
#include "PanelConfigurator.h"

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
    [[nodiscard]] PanoramaUiPanel get(Hud<Dependencies> hud, PanelConfigurator panelConfigurator) noexcept
    {
        if (const auto container = containerPanel.get())
            return container;
        return createPanel(hud, panelConfigurator);
    }

private:
    template <typename Dependencies>
    [[nodiscard]] PanoramaUiPanel createPanel(Hud<Dependencies> hud, PanelConfigurator panelConfigurator) noexcept
    {
        if (const auto hudReticle = hud.getHudReticle()) {
            if (const auto panel = Panel::create("", hudReticle)) {
                if (const auto style{PanoramaUiPanel{panel->uiPanel}.getStyle()})
                    panelConfigurator.panelStyle(*style).fitParent();
                containerPanel = panel->uiPanel;
                return PanoramaUiPanel{ panel->uiPanel };
            }
        }
        return PanoramaUiPanel{ nullptr };
    }

    PanoramaPanelPointer containerPanel;
};
