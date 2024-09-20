#pragma once

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

private:
    HookContext& hookContext;
};
