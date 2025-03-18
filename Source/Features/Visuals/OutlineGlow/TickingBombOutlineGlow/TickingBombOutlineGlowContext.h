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
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<TickingBombOutlineGlowCondition<TickingBombOutlineGlowContext>>();
    }

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return hookContext.config();
    }

private:
    HookContext& hookContext;
};
