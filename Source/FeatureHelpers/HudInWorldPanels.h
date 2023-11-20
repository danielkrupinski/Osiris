#pragma once

#include "HudInWorldPanelFactory.h"

template <typename T>
class HudInWorldPanels {
public:
    void createPanels(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        if (containerPanelPointer.get())
            return;

        const auto containerPanel = T::createContainerPanel(inWorldFactory);
        if (!containerPanel)
            return;

        containerPanelPointer = containerPanel->uiPanel;
        T::createContentPanels(*containerPanel->uiPanel);
    }

    [[nodiscard]] PanoramaUiPanel getPanel(std::size_t index) const noexcept
    {
        const auto containerPanel = containerPanelPointer.get();
        if (!containerPanel)
            return PanoramaUiPanel{ nullptr };

        if (const auto children = containerPanel.children()) {
            if (children->size > 0 && static_cast<std::size_t>(children->size) > index)
                return PanoramaUiPanel{ children->memory[index] };
        }
        return PanoramaUiPanel{ nullptr };
    }

    void hidePanels(std::size_t fromPanelIndex) const noexcept
    {
        for (std::size_t i = fromPanelIndex; i < T::kMaxNumberOfPanels; ++i) {
            const auto panel = getPanel(i);
            if (!panel)
                break;

            if (const auto style = panel.getStyle())
                style.setOpacity(0.0f);
        }
    }

private:
    PanoramaPanelPointer containerPanelPointer;
};
