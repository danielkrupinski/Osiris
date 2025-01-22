#pragma once

#include <concepts>
#include <Utils/PrivateTag.h>

template <typename Feature, bool HasOnEnable, bool HasOnDisable>
class FeatureToggleBase {
public:
    using ToggleMethod = PrivateTag<FeatureToggleBase>;

    void enable() noexcept
    {
        if (auto& enabled = static_cast<Feature&>(*this).enabledVariable(ToggleMethod{}); !enabled) {
            enabled = true;
            callOnEnable();
        }
    }

    void disable() noexcept
    {
        if (auto& enabled = static_cast<Feature&>(*this).enabledVariable(ToggleMethod{})) {
            enabled = false;
            callOnDisable();
        }
    }

private:
    void callOnEnable() noexcept
    {
        static_assert(HasOnEnable == requires (Feature& feature) { { feature.onEnable(ToggleMethod{}) } -> std::same_as<void>; });
        if constexpr (HasOnEnable)
            static_cast<Feature&>(*this).onEnable(ToggleMethod{});
    }

    void callOnDisable() noexcept
    {
        static_assert(HasOnDisable == requires (Feature& feature) { { feature.onDisable(ToggleMethod{}) } -> std::same_as<void>; });
        if constexpr (HasOnDisable)
            static_cast<Feature&>(*this).onDisable(ToggleMethod{});
    }
};

template <typename Feature>
using FeatureToggleOnOff = FeatureToggleBase<Feature, true, true>;

template <typename Feature>
using FeatureToggleOn = FeatureToggleBase<Feature, true, false>;

template <typename Feature>
using FeatureToggleOff = FeatureToggleBase<Feature, false, true>;

template <typename Feature>
using FeatureToggle = FeatureToggleBase<Feature, false, false>;
