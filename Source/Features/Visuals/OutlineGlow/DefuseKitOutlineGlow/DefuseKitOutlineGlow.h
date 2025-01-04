#pragma once

#include <utility>

#include "DefuseKitOutlineGlowContext.h"
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>

template <typename HookContext, typename Context = DefuseKitOutlineGlowContext<HookContext>>
class DefuseKitOutlineGlow {
public:
    template <typename... Args>
    DefuseKitOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }
    
    void applyGlowToDefuseKit(auto&& defuseKit) const noexcept
    {
        if (context.state().enabledForDefuseKits) {
            using namespace outline_glow_params;
            defuseKit.applyGlowRecursively(kDefuseKitColor, kDefuseKitGlowRange);
        }
    }

private:
    Context context;
};
