#pragma once

#include <utility>

#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include "HostageOutlineGlowContext.h"

template <typename HookContext, typename Context = HostageOutlineGlowContext<HookContext>>
class HostageOutlineGlow {
public:
    template <typename... Args>
    HostageOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToHostage(auto&& hostage) const noexcept
    {
        if (context.state().enabledForHostages) {
            hostage.applyGlowRecursively(outline_glow_params::kHostageColor);
        }
    }

private:
    Context context;
};
