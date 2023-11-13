#pragma once

#include <CS2/Constants/PanelIDs.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaPanelPointer.h>

struct HudScopePanels {
    void updatePanelPointers(HudProvider hudProvider) noexcept
    {
        if (!shouldUpdatePanelPointers())
            return;

        if (const auto hudScopePanel = getHudScopePanel(hudProvider)) {
            scopeCirclePointer = hudScopePanel.findChildInLayoutFile(cs2::ScopeCircle);
            scopeDustPointer = hudScopePanel.findChildInLayoutFile(cs2::ScopeDust);
        }
    }

    void setPanelsVisible(bool visible) const noexcept
    {
        if (const auto scopeCirclePanel = scopeCirclePointer.get())
            scopeCirclePanel.setVisible(visible);

        if (const auto scopeDustPanel = scopeDustPointer.get())
            scopeDustPanel.setVisible(visible);
    }

private:
    [[nodiscard]] bool shouldUpdatePanelPointers() const noexcept
    {
        return !scopeCirclePointer.get();
    }

    [[nodiscard]] PanoramaUiPanel getHudScopePanel(HudProvider hudProvider) const noexcept
    {
        if (auto hudScopePanel = hudProvider.findChildInLayoutFile(cs2::HudScope))
            return hudScopePanel;
        return mainMenu.findChildInLayoutFile(cs2::HudScope);
    }

    PanoramaUiPanel mainMenu{ ClientPatterns::mainMenuPanel()->uiPanel };
    PanoramaPanelPointer scopeCirclePointer;
    PanoramaPanelPointer scopeDustPointer;
};
