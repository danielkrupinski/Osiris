#pragma once

#include "LoopModeGameHook.h"
#include "ViewRenderHook.h"

#include <Vmt/VmtLengthCalculator.h>

struct Hooks {
    explicit Hooks(const VmtLengthCalculator& clientVmtLengthCalculator) noexcept
        : loopModeGameHook{ clientVmtLengthCalculator }
        , viewRenderHook{ clientVmtLengthCalculator }
    {
    }

    LoopModeGameHook loopModeGameHook;
    ViewRenderHook viewRenderHook;
};
