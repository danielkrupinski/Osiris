#pragma once

#include "LoopModeGameHook.h"
#include "PeepEventsHook.h"
#include "ViewRenderHook.h"

#include <Vmt/VmtLengthCalculator.h>

struct Hooks {
    Hooks(PeepEventsHook peepEventsHook, const ClientPatterns& clientPatterns, const VmtLengthCalculator& clientVmtLengthCalculator) noexcept
        : peepEventsHook{peepEventsHook}
        , loopModeGameHook{clientPatterns, clientVmtLengthCalculator}
        , viewRenderHook{clientPatterns, clientVmtLengthCalculator}
    {
    }

    void update() noexcept
    {
        loopModeGameHook.update();
        viewRenderHook.update();
    }

    void forceUninstall() noexcept
    {
        peepEventsHook.disable();
        loopModeGameHook.forceUninstall();
        viewRenderHook.forceUninstall();
    }

    PeepEventsHook peepEventsHook;
    LoopModeGameHook loopModeGameHook;
    ViewRenderHook viewRenderHook;
};
