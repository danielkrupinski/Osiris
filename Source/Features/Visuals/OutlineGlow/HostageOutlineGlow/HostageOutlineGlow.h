#pragma once

#include <utility>

#include "HostageOutlineGlowContext.h"
#include "HostageOutlineGlowParams.h"


template <typename Context>
class HostageOutlineGlow {
public:
    template <typename... Args>
    HostageOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToHostage(auto&& hostage) const noexcept
    {
        if (context.state().enabled) {
            using namespace hostage_outline_glow_params;
            hostage.applyGlowRecursively(kColor);
        }
    }

private:
    Context context;
};

template <typename HookContext>
HostageOutlineGlow(HookContext&) -> HostageOutlineGlow<HostageOutlineGlowContext<HookContext>>;
