#pragma once

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

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return hookContext.config();
    }

private:
    HookContext& hookContext;
};
