#pragma once

#include <cassert>

#include "GlobalVars.h"
#include "GlobalVarsCache.h"

struct GlobalVarsOptional {
    GlobalVarsOptional(cs2::GlobalVars** pointerToGlobalVars, GlobalVarsCache& globalVarsCache) noexcept
        : pointerToGlobalVars{pointerToGlobalVars}
        , globalVars{globalVarsCache}
    {
    }

    cs2::GlobalVars** pointerToGlobalVars;
    GlobalVars globalVars;

    [[nodiscard]] operator bool() const noexcept
    {
        return globalVars.cache.globalVars || readGlobalVars();
    }

    [[nodiscard]] const GlobalVars* operator->() const noexcept
    {
        assert(static_cast<bool>(*this));
        return &globalVars;
    }

private:
    [[nodiscard]] bool readGlobalVars() const noexcept
    {
        return pointerToGlobalVars && ((globalVars.cache.globalVars = *pointerToGlobalVars) != nullptr);
    }
};
    