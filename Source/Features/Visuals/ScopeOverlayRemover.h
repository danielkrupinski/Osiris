#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>

#include <Hooks/LoopModeGameHook.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <GameClasses/ClientMode.h>

#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/Visuals/SniperScopeBlurRemover.h>

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

class ScopeOverlayRemover : public TogglableFeature<ScopeOverlayRemover> {
public:
    ScopeOverlayRemover(ScopeOverlayRemoverState& state, PanoramaUiPanel mainMenu, cs2::CPanel2D** hudScope, LoopModeGameHook& loopModeGameHook, SniperScopeBlurRemover& sniperScopeBlurRemover) noexcept
        : TogglableFeature{state.enabled}
        , state{state}
        , mainMenu{mainMenu}
        , hudScope{hudScope}
        , loopModeGameHook{loopModeGameHook}
        , sniperScopeBlurRemover{sniperScopeBlurRemover}
    {
    }

    void updatePanelVisibility(HudProvider hudProvider) noexcept
    {
        if (isEnabled()) {
            hidePanels(hudProvider);
            removeScopeStencil();
        }
    }


private:    
    friend TogglableFeature;

    void onEnable() const noexcept
    {
        sniperScopeBlurRemover.incrementReferenceCount(loopModeGameHook);
    }

    void onDisable() const noexcept
    {
        sniperScopeBlurRemover.decrementReferenceCount(loopModeGameHook);
        state.restorePanels();
    }

    void hidePanels(HudProvider hudProvider) noexcept
    {
        updatePanelPointers(hudProvider);
        state.setPanelsVisible(false);
    }

    void removeScopeStencil() noexcept
    {
        if (hudScope)
            *hudScope = nullptr;
    }

    void updatePanelPointers(HudProvider hudProvider) noexcept
    {
        if (!shouldUpdatePanelPointers())
            return;

        if (const auto hudScopePanel = getHudScopePanel(hudProvider)) {
            state.scopeCirclePointer = hudScopePanel.findChildInLayoutFile(cs2::ScopeCircle);
            state.scopeDustPointer = hudScopePanel.findChildInLayoutFile(cs2::ScopeDust);
        }
    }

    [[nodiscard]] bool shouldUpdatePanelPointers() const noexcept
    {
        return !state.scopeCirclePointer.get();
    }

    [[nodiscard]] PanoramaUiPanel getHudScopePanel(HudProvider hudProvider) const noexcept
    {
        if (auto hudScopePanel = hudProvider.findChildInLayoutFile(cs2::HudScope))
            return hudScopePanel;
        return mainMenu.findChildInLayoutFile(cs2::HudScope);
    }

    ScopeOverlayRemoverState& state;
    PanoramaUiPanel mainMenu;
    cs2::CPanel2D** hudScope;
    LoopModeGameHook& loopModeGameHook;
    SniperScopeBlurRemover& sniperScopeBlurRemover;
};
