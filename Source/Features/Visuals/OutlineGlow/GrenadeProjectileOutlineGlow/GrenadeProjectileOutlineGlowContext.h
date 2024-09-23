#pragma once

#include "GrenadeProjectileOutlineGlowCondition.h"

template <typename HookContext>
class GrenadeProjectileOutlineGlowContext {
public:
    explicit GrenadeProjectileOutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.grenadeProjectileOutlineGlowState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<GrenadeProjectileOutlineGlowCondition<GrenadeProjectileOutlineGlowContext>>();
    }

private:
    HookContext& hookContext;
};
