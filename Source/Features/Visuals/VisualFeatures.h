#pragma once

#include "ScopeOverlayRemover/ScopeOverlayRemover.h"

class LoopModeGameHook;

struct VisualFeatures {
    VisualFeatures(LoopModeGameHook& loopModeGameHook) noexcept
        : scopeOverlayRemover{ loopModeGameHook }
    {
    }

    ScopeOverlayRemover scopeOverlayRemover;
};
