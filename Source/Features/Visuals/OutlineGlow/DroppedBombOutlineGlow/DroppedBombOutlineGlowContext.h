#pragma once

#include "DroppedBombOutlineGlowCondition.h"

template <typename HookContext>
class DroppedBombOutlineGlowContext {
public:
    explicit DroppedBombOutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.droppedBombOutlineGlowState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<DroppedBombOutlineGlowCondition<DroppedBombOutlineGlowContext>>();
    }

private:
    HookContext& hookContext;
};
