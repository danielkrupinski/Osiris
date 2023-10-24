#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/PanelIDs.h>

#include <Hooks/LoopModeGameHook.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <GameClasses/PanoramaUiPanel.h>
#include <GameClasses/ClientMode.h>

#include <FeatureHelpers/TogglableFeature.h>

#include "HudScopePanels.h"

class ScopeOverlayRemover : public TogglableFeature<ScopeOverlayRemover> {
public:
    ScopeOverlayRemover(HudProvider hudProvider, LoopModeGameHook& loopModeGameHook) noexcept
        : hudScopePanels{ hudProvider }, loopModeGameHook{ loopModeGameHook }
    {
    }
        
    void getWorldSessionHook(ReturnAddress returnAddress) const noexcept
    {
        if (shouldRemoveZoomedSniperEffect(returnAddress))
            clientMode.removeZoomedSniperEffect();
    }

    void updatePanelVisibility() noexcept
    {
        if (isEnabled()) {
            hidePanels();
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
        loopModeGameHook.incrementReferenceCount();
    }

    void onDisable() const noexcept
    {
        loopModeGameHook.decrementReferenceCount();
        restorePanels();
    }

    void hidePanels() noexcept
    {
        hudScopePanels.updatePanelPointers();
        hudScopePanels.setPanelsVisible(false);
    }

    void removeScopeStencil() noexcept
    {
        if (hudScope)
            *hudScope = nullptr;
    }

    [[nodiscard]] bool shouldRemoveZoomedSniperEffect(ReturnAddress returnAddress) const noexcept
    {
        return isEnabled() && returnAddress == getWorldSessionInClientMode && clientMode;
    }

    void restorePanels() const noexcept
    {
        hudScopePanels.setPanelsVisible(true);
    }

    cs2::CPanel2D** hudScope{ ClientPatterns::hudScope() };
    ReturnAddress getWorldSessionInClientMode{ ClientPatterns::getWorldSessionInClientMode() };
    ClientMode clientMode{ ClientPatterns::clientMode() };

    HudScopePanels hudScopePanels;
    LoopModeGameHook& loopModeGameHook;
};
