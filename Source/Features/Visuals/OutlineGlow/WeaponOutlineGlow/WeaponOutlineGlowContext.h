#pragma once

#include "WeaponOutlineGlowCondition.h"

template <typename HookContext>
class WeaponOutlineGlowContext {
public:
    explicit WeaponOutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<WeaponOutlineGlowCondition<WeaponOutlineGlowContext>>();
    }

private:
    HookContext& hookContext;
};
