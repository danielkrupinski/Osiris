#pragma once

#include "ScopeOverlayRemover/ScopeOverlayRemover.h"
#include "SniperScopeBlurRemovalFeature.h"

class LoopModeGameHook;
class SniperScopeBlurRemover;

struct VisualFeatures {
    VisualFeatures(SniperScopeBlurRemover& sniperScopeBlurRemover, LoopModeGameHook& loopModeGameHook) noexcept
        : scopeOverlayRemover{loopModeGameHook, sniperScopeBlurRemover}
        , sniperScopeBlurRemoval{loopModeGameHook, sniperScopeBlurRemover}
    {
    }

    ScopeOverlayRemover scopeOverlayRemover;
    SniperScopeBlurRemovalFeature sniperScopeBlurRemoval;
};
