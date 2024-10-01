#pragma once

#include "PlayerInfoInWorldCondition.h"

template <typename HookContext>
class PlayerInfoInWorldContext {
public:
    explicit PlayerInfoInWorldContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<PlayerInfoInWorldCondition<PlayerInfoInWorldContext>>();
    }

private:
    HookContext& hookContext;
};
