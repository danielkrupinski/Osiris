#pragma once

#include <utility>

#include "DefuseKitOutlineGlowContext.h"
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>

template <typename HookContext>
class DefuseKitOutlineGlow {
public:
    explicit DefuseKitOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }
    
    void applyGlowToDefuseKit(auto&& defuseKit) const noexcept
    {
        if (hookContext.config().template getVariable<DefuseKitOutlineGlowEnabled>()) {
            using namespace outline_glow_params;
            defuseKit.applyGlowRecursively(kDefuseKitColor, kDefuseKitGlowRange);
        }
    }

private:
    HookContext& hookContext;
};
