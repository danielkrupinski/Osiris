#pragma once

#include <concepts>

template <typename Feature>
class TogglableFeature {
public:
    explicit TogglableFeature(bool& enabled) noexcept
        : enabled{enabled}
    {
    }

    void enable() noexcept
    {
        if (!enabled) {
            enabled = true;
            callOnEnable();
        }
    }

    void disable() noexcept
    {
        if (enabled) {
            enabled = false;
            callOnDisable();
        }
    }

    [[nodiscard]] bool isEnabled() const noexcept
    {
        return enabled;
    }

private:
    void callOnEnable() noexcept
    {
        if constexpr (requires (Feature& feature) { { feature.onEnable() } -> std::same_as<void>; })
            static_cast<Feature&>(*this).onEnable();
    }

    void callOnDisable() noexcept
    {
        if constexpr (requires (Feature& feature) { { feature.onDisable() } -> std::same_as<void>; })
            static_cast<Feature&>(*this).onDisable();
    }

    bool& enabled;
};
