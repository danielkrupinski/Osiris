#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>

#include <Hooks/LoopModeGameHook.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <GameClasses/ClientMode.h>

#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/Visuals/SniperScopeBlurRemover.h>

#include "HudScopePanels.h"

class ScopeOverlayRemover : public TogglableFeature<ScopeOverlayRemover> {
public:
    ScopeOverlayRemover(LoopModeGameHook& loopModeGameHook, SniperScopeBlurRemover& sniperScopeBlurRemover) noexcept
        : loopModeGameHook{loopModeGameHook}
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

    ~ScopeOverlayRemover() noexcept
    {
        restorePanels();
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
        restorePanels();
    }

    void hidePanels(HudProvider hudProvider) noexcept
    {
        hudScopePanels.updatePanelPointers(hudProvider);
        hudScopePanels.setPanelsVisible(false);
    }

    void removeScopeStencil() noexcept
    {
        if (hudScope)
            *hudScope = nullptr;
    }

    void restorePanels() const noexcept
    {
        hudScopePanels.setPanelsVisible(true);
    }

    cs2::CPanel2D** hudScope{ ClientPatterns::hudScope() };
    LoopModeGameHook& loopModeGameHook;
    SniperScopeBlurRemover& sniperScopeBlurRemover;
    HudScopePanels hudScopePanels;
};
