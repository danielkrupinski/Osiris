#pragma once

#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <Helpers/HudProvider.h>
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

    [[nodiscard]] PanoramaUiPanel get(HudProvider hudProvider) noexcept
    {
        if (const auto container = containerPanel.get())
            return container;
        return createPanel(hudProvider);
    }

private:
    [[nodiscard]] PanoramaUiPanel createPanel(HudProvider hudProvider) noexcept
    {
        if (const auto hudReticle = hudProvider.getHudReticle()) {
            if (const auto panel = Panel::create("", hudReticle)) {
                PanoramaUiPanel{ panel->uiPanel }.fitParent();
                containerPanel = panel->uiPanel;
                return PanoramaUiPanel{ panel->uiPanel };
            }
        }
        return PanoramaUiPanel{ nullptr };
    }

    PanoramaPanelPointer containerPanel;
};
