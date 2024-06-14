#pragma once

#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <Helpers/HudProvider.h>
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

    [[nodiscard]] PanoramaUiPanel get(HudOptional hud, PanelConfigurator panelConfigurator) noexcept
    {
        if (const auto container = containerPanel.get())
            return container;
        return createPanel(hud, panelConfigurator);
    }

private:
    [[nodiscard]] PanoramaUiPanel createPanel(HudOptional hud, PanelConfigurator panelConfigurator) noexcept
    {
        if (!hud)
            return PanoramaUiPanel{nullptr};

        if (const auto hudReticle = hud->getHudReticle()) {
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
