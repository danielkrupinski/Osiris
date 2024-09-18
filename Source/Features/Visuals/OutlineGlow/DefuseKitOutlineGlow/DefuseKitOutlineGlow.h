#pragma once

#include <utility>

#include "DefuseKitOutlineGlowContext.h"
#include "DefuseKitOutlineGlowParams.h"

template <typename Context>
class DefuseKitOutlineGlow {
public:
    template <typename... Args>
    DefuseKitOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }
    
    void applyGlowToDefuseKit(auto&& defuseKit) const noexcept
    {
        if (context.state().enabled) {
            using namespace defuse_kit_outline_glow_params;
            defuseKit.applyGlowRecursively(kColor, kRange);
        }
    }

private:
    Context context;
};

template <typename HookContext>
DefuseKitOutlineGlow(HookContext&) -> DefuseKitOutlineGlow<DefuseKitOutlineGlowContext<HookContext>>;
