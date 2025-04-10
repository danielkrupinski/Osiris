#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowConfigVariables.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>

template <typename HookContext>
class HostageOutlineGlow {
public:
    explicit HostageOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void applyGlowToHostage(auto&& hostage) const noexcept
    {
        if (hookContext.config().template getVariable<HostageOutlineGlowEnabled>())
            hostage.applyGlowRecursively(outline_glow_params::kHostageColor);
    }

private:
    HookContext& hookContext;
};
