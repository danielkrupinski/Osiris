#pragma once

template <typename HookContext>
class DefuseKitOutlineGlowContext {
public:
    explicit DefuseKitOutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

private:
    HookContext& hookContext;
};
