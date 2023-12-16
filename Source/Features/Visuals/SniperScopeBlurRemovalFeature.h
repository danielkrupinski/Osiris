#pragma once

#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/Visuals/SniperScopeBlurRemover.h>

class LoopModeGameHook;

class SniperScopeBlurRemovalFeature : public TogglableFeature<SniperScopeBlurRemovalFeature> {
public:
    SniperScopeBlurRemovalFeature(LoopModeGameHook& loopModeGameHook, SniperScopeBlurRemover& sniperScopeBlurRemover) noexcept
        : loopModeGameHook{loopModeGameHook}
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

    LoopModeGameHook& loopModeGameHook;
    SniperScopeBlurRemover& sniperScopeBlurRemover;
};
