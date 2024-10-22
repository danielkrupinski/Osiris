#pragma once

#include "PeepEventsHook.h"
#include "ViewRenderHook.h"

#include <Vmt/VmtLengthCalculator.h>

struct Hooks {
    Hooks(PeepEventsHook peepEventsHook, cs2::CViewRender** viewRender, const VmtLengthCalculator& clientVmtLengthCalculator) noexcept
        : peepEventsHook{peepEventsHook}
        , viewRenderHook{viewRender, clientVmtLengthCalculator}
    {
    }

    void update() noexcept
    {
        viewRenderHook.update();
    }

    void forceUninstall() noexcept
    {
        peepEventsHook.disable();
        viewRenderHook.forceUninstall();
    }

    PeepEventsHook peepEventsHook;
    ViewRenderHook viewRenderHook;
};
