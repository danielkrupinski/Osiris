#pragma once

#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/Visuals/SniperScopeBlurRemover.h>

#include "States/SniperScopeBlurRemovalFeatureState.h"

class LoopModeGameHook;

class SniperScopeBlurRemovalFeature : public TogglableFeature<SniperScopeBlurRemovalFeature> {
public:
    SniperScopeBlurRemovalFeature(SniperScopeBlurRemovalFeatureState& state, LoopModeGameHook& loopModeGameHook, SniperScopeBlurRemover& sniperScopeBlurRemover) noexcept
        : TogglableFeature{state.enabled}
        , state{state}
        , loopModeGameHook{loopModeGameHook}
        , sniperScopeBlurRemover{sniperScopeBlurRemover}
    {
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
    }

    SniperScopeBlurRemovalFeatureState& state;
    LoopModeGameHook& loopModeGameHook;
    SniperScopeBlurRemover& sniperScopeBlurRemover;
};
