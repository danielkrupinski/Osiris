#pragma once

#include "HudInWorldPanelFactory.h"
#include "PanelConfigurator.h"

template <typename T>
class HudInWorldPanels {
public:
    [[nodiscard]] PanoramaUiPanel getPanel(int index, const HudInWorldPanelFactory& inWorldFactory, PanelConfigurator panelConfigurator) noexcept
    {
        auto containerPanel = containerPanelPointer.get();
        if (!containerPanel) {
            if (const auto container{T::createContainerPanel(inWorldFactory)}) {
                containerPanelPointer = containerPanel = PanoramaUiPanel{container->uiPanel};
            }
        }

        if (!containerPanel)
            return PanoramaUiPanel{nullptr};

        if (const auto children{containerPanel.children()}) {
            if (index >= 0 && index < children->size)
                return PanoramaUiPanel{children->memory[index]};
            return T::createContentPanel(containerPanel, panelConfigurator);
        }
        return PanoramaUiPanel{nullptr};
    }

    void hidePanels(int fromPanelIndex, PanelConfigurator panelConfigurator) const noexcept
    {
        const auto containerPanel = containerPanelPointer.get();
        if (!containerPanel)
            return;

        if (const auto children{containerPanel.children()}) {
            for (int i{fromPanelIndex}; i < children->size; ++i) {
                if (const auto style{PanoramaUiPanel{children->memory[i]}.getStyle()})
                    panelConfigurator.panelStyle(*style).setOpacity(0.0f);
            }
        }
    }

private:
    PanoramaPanelPointer containerPanelPointer;
};
