#pragma once

#include "Hooks/PeepEventsHook.h"

struct PeepEventsHookResult {
    cs2::SDL_PeepEvents original;
    bool shouldUnload;
};
