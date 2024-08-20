#pragma once

#include <cassert>
#include <limits>
#include <utility>

#include <CS2/Panorama/CUIPanel.h>
#include <GameClasses/PanoramaUiPanel.h>

#include "HudInWorldPanelIndex.h"

struct HudInWorldPanels {
    [[nodiscard]] auto getPanel(HudInWorldPanelIndex index, auto& hookContext) const noexcept
    {
        if (std::cmp_less(index, containerPanelChildren.size))
            return PanoramaUiPanel{PanoramaUiPanelContext{hookContext, containerPanelChildren.memory[index]}};
        return PanoramaUiPanel{PanoramaUiPanelContext{hookContext, nullptr}};
    }

    [[nodiscard]] HudInWorldPanelIndex getIndexOfLastPanel() const noexcept
    {
        if (containerPanelChildren.size > 0) {
            assert(containerPanelChildren.size <= (std::numeric_limits<HudInWorldPanelIndex>::max)());
            return static_cast<HudInWorldPanelIndex>(containerPanelChildren.size - 1);
        }
        return 0;
    }

    const cs2::CUIPanel::childrenVector& containerPanelChildren;
};
