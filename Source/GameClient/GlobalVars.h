#pragma once

#include <CS2/Classes/GlobalVars.h>
#include <Utils/Optional.h>

template <typename HookContext>
struct GlobalVars {
    [[nodiscard]] Optional<float> curtime() const noexcept
    {
        if (globalVars)
            return globalVars->curtime;
        return {};
    }

    [[nodiscard]] Optional<float> frametime() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToFrametime>().of(globalVars).toOptional();
    }

    HookContext& hookContext;
    cs2::GlobalVars* globalVars;
};
