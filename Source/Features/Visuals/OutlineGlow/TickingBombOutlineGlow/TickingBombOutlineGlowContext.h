#pragma once

#include "TickingBombOutlineGlowCondition.h"

template <typename HookContext>
class TickingBombOutlineGlowContext {
public:
    explicit TickingBombOutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.tickingBombOutlineGlowState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<TickingBombOutlineGlowCondition<TickingBombOutlineGlowContext>>();
    }

private:
    HookContext& hookContext;
};
