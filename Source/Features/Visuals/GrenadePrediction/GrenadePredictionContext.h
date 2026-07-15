#pragma once

#include "GrenadePredictionCondition.h"

template <typename HookContext>
class GrenadePredictionContext {
public:
    explicit GrenadePredictionContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.grenadePredictionState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<GrenadePredictionCondition<GrenadePredictionContext>>();
    }

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return hookContext.config();
    }

private:
    HookContext& hookContext;
};
