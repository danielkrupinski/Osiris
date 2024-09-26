#pragma once

template <typename HookContext>
class HostageOutlineGlowContext {
public:
    explicit HostageOutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.hostageOutlineGlowState;
    }

private:
    HookContext& hookContext;
};
