#pragma once

#include "PeepEventsHook.h"
#include "ViewRenderHook.h"

#include <CS2/Classes/CViewRender.h>
#include <Vmt/VmtLengthCalculator.h>

struct Hooks {
    Hooks(PeepEventsHook peepEventsHook, cs2::CViewRender** viewRender, const VmtLengthCalculator& clientVmtLengthCalculator) noexcept
        : clientVmtLengthCalculator{clientVmtLengthCalculator}
        , peepEventsHook{peepEventsHook}
        , viewRenderHook{viewRender, clientVmtLengthCalculator}
    {
    }

    VmtLengthCalculator clientVmtLengthCalculator;
    VmtSwapper clientModeVmtSwapper;
    cs2::ClientModeCSNormal::GetViewmodelFov* originalGetViewmodelFov{nullptr};
    PeepEventsHook peepEventsHook;
    ViewRenderHook viewRenderHook;
};
