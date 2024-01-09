#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>
#include <GameClasses/Panel.h>
#include <Helpers/HudProvider.h>

#include "HudInWorldPanelContainer.h"
#include "HudInWorldPanelZOrder.h"

class HudInWorldPanelFactory {
public:
    HudInWorldPanelFactory(HudInWorldPanelContainer& container, HudProvider hudProvider, PanelConfigurator panelConfigurator) noexcept
        : container{container}
        , hudProvider{hudProvider}
        , panelConfigurator{panelConfigurator}
    {
    }

    [[nodiscard]] cs2::CPanel2D* createPanel(const char* id, HudInWorldPanelZOrder zIndex) const noexcept
    {
        if (const auto containerPanel = container.get(hudProvider, panelConfigurator))
            return createChild(id, containerPanel, zIndex);
        return nullptr;
    }

private:
    [[nodiscard]] cs2::CPanel2D* createChild(const char* id, cs2::CUIPanel* hudReticle, HudInWorldPanelZOrder zIndex) const noexcept
    {
        if (const auto panel = Panel::create(id, hudReticle)) {
            if (const auto style = PanoramaUiPanel{panel->uiPanel}.getStyle()) {
                const auto panelStyle{panelConfigurator.panelStyle(*style)};
                panelStyle.fitParent();
                panelStyle.setZIndex(static_cast<float>(zIndex));
            }
            return panel;
        }
        return nullptr;
    }

    HudInWorldPanelContainer& container;
    HudProvider hudProvider;
    PanelConfigurator panelConfigurator;
};
