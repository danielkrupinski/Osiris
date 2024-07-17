#pragma once

#include "LoopModeGameHook.h"
#include "PeepEventsHook.h"
#include "ViewRenderHook.h"

#include <Vmt/VmtLengthCalculator.h>

struct Hooks {
    Hooks(PeepEventsHook peepEventsHook, cs2::CLoopModeGame** loopModeGame, cs2::CViewRender** viewRender, const VmtLengthCalculator& clientVmtLengthCalculator) noexcept
        : peepEventsHook{peepEventsHook}
        , loopModeGameHook{loopModeGame, clientVmtLengthCalculator}
        , viewRenderHook{viewRender, clientVmtLengthCalculator}
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
