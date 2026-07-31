#pragma once

#include "CreateMoveHook.h"
#include "PeepEventsHook.h"
#include "ViewRenderHook.h"

#include <CS2/Classes/CViewRender.h>
#include <Vmt/VmtLengthCalculator.h>

struct Hooks {
    Hooks(PeepEventsHook peepEventsHook, cs2::CViewRender** viewRender, const VmtLengthCalculator& clientVmtLengthCalculator) noexcept
        : clientVmtLengthCalculator{clientVmtLengthCalculator}
        , peepEventsHook{peepEventsHook}
        , viewRenderHook{viewRender, clientVmtLengthCalculator}
        , createMoveHook{clientVmtLengthCalculator}
    {
    }

    VmtLengthCalculator clientVmtLengthCalculator;
    VmtSwapper clientModeVmtSwapper;
    cs2::ClientModeCSNormal::GetViewmodelFov* originalGetViewmodelFov{nullptr};
    cs2::ClientModeCSNormal::OverrideView* originalOverrideView{nullptr};
    bool getViewmodelFovHookInstalled{false};
    bool overrideViewHookInstalled{false};
    PeepEventsHook peepEventsHook;
    ViewRenderHook viewRenderHook;
    CreateMoveHook createMoveHook;
    // Silent aim IPC — set by Aimbot in render hook, read by CreateMove hook.
    float silentAimPitch{0.0f};
    float silentAimYaw{0.0f};
    bool  silentAimActive{false};
};
