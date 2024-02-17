#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>

#include <Hooks/LoopModeGameHook.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <GameClasses/PanoramaUiPanel.h>

#include <FeatureHelpers/MainMenuProvider.h>
#include <FeatureHelpers/TogglableFeature.h>

#include "States/ScopeOverlayRemoverState.h"

class ScopeOverlayRemover : public TogglableFeature<ScopeOverlayRemover> {
public:
    ScopeOverlayRemover(ScopeOverlayRemoverState& state, MainMenuProvider mainMenuProvider, HudProvider hudProvider, cs2::CPanel2D** hudScope) noexcept
        : TogglableFeature{state.enabled}
        , state{state}
        , mainMenuProvider{mainMenuProvider}
        , hudProvider{hudProvider}
        , hudScope{hudScope}
    {
    }

    void updatePanelVisibility() noexcept
    {
        if (isEnabled()) {
            hidePanels();
            removeScopeStencil();
        }
    }

private:    
    friend TogglableFeature;

    void onDisable() const noexcept
    {
        state.restorePanels();
    }

    void hidePanels() noexcept
    {
        updatePanelPointers();
        state.setPanelsVisible(false);
    }

    void removeScopeStencil() noexcept
    {
        if (hudScope)
            *hudScope = nullptr;
    }

    void updatePanelPointers() noexcept
    {
        if (!shouldUpdatePanelPointers())
            return;

        if (const auto hudScopePanel = getHudScopePanel()) {
            state.scopeCirclePointer = hudScopePanel.findChildInLayoutFile(cs2::ScopeCircle);
            state.scopeDustPointer = hudScopePanel.findChildInLayoutFile(cs2::ScopeDust);
        }
    }

    [[nodiscard]] bool shouldUpdatePanelPointers() const noexcept
    {
        return !state.scopeCirclePointer.get();
    }

    [[nodiscard]] PanoramaUiPanel getHudScopePanel() const noexcept
    {
        if (auto hudScopePanel = hudProvider.findChildInLayoutFile(cs2::HudScope))
            return hudScopePanel;
        if (const auto mainMenu{mainMenuProvider.getMainMenuPanel()})
            return mainMenu.findChildInLayoutFile(cs2::HudScope);
        return PanoramaUiPanel{nullptr};
    }

    ScopeOverlayRemoverState& state;
    MainMenuProvider mainMenuProvider;
    HudProvider hudProvider;
    cs2::CPanel2D** hudScope;
};
