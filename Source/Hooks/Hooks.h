#pragma once

#include "LoopModeGameHook.h"
#include "ViewRenderHook.h"

#include <Vmt/VmtLengthCalculator.h>

struct Hooks {
    explicit Hooks(const ClientPatterns& clientPatterns, const VmtLengthCalculator& clientVmtLengthCalculator) noexcept
        : loopModeGameHook{clientPatterns, clientVmtLengthCalculator}
        , viewRenderHook{clientPatterns, clientVmtLengthCalculator}
    {
    }

    LoopModeGameHook loopModeGameHook;
    ViewRenderHook viewRenderHook;
};
