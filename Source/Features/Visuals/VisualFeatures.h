#pragma once

#include "ScopeOverlayRemover/ScopeOverlayRemover.h"
#include "SniperScopeBlurRemovalFeature.h"

class LoopModeGameHook;
class SniperScopeBlurRemover;

struct VisualFeatures {
    VisualFeatures(const ClientPatterns& clientPatterns, SniperScopeBlurRemover& sniperScopeBlurRemover, LoopModeGameHook& loopModeGameHook) noexcept
        : scopeOverlayRemover{clientPatterns, loopModeGameHook, sniperScopeBlurRemover}
        , sniperScopeBlurRemoval{loopModeGameHook, sniperScopeBlurRemover}
    {
    }

    ScopeOverlayRemover scopeOverlayRemover;
    SniperScopeBlurRemovalFeature sniperScopeBlurRemoval;
};
