#pragma once

#include "PlayerOutlineGlowCondition.h"

template <typename HookContext>
class PlayerOutlineGlowContext {
public:
    explicit PlayerOutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<PlayerOutlineGlowCondition>();
    }

private:
    HookContext& hookContext;
};
