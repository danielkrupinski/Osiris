#pragma once

#include "ScopeOverlayRemover/ScopeOverlayRemover.h"

class LoopModeGameHook;

struct VisualFeatures {
    VisualFeatures(HudProvider hudProvider, LoopModeGameHook& loopModeGameHook) noexcept
        : scopeOverlayRemover{ hudProvider, loopModeGameHook }
    {
    }

    ScopeOverlayRemover scopeOverlayRemover;
};
