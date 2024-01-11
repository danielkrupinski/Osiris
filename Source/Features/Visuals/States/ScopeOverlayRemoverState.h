#pragma once

#include <Helpers/PanoramaPanelPointer.h>

struct ScopeOverlayRemoverState {
    bool enabled{false};
    
    PanoramaPanelPointer scopeCirclePointer;
    PanoramaPanelPointer scopeDustPointer;

    ~ScopeOverlayRemoverState() noexcept
    {
        restorePanels();
    }

    void restorePanels() const noexcept
    {
        setPanelsVisible(true);
    }

    void setPanelsVisible(bool visible) const noexcept
    {
        if (const auto scopeCirclePanel = scopeCirclePointer.get())
            scopeCirclePanel.setVisible(visible);

        if (const auto scopeDustPanel = scopeDustPointer.get())
            scopeDustPanel.setVisible(visible);
    }
};
