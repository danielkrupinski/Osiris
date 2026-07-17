#pragma once

#include <CS2/Classes/Color.h>
#include <Config/ConfigVariable.h>
#include <Config/RangeConstrainedVariableParams.h>
#include <cstdint>

namespace grenade_prediction_vars
{
    [[nodiscard]] constexpr float normalizeCacheDuration(float value) noexcept
    {
        constexpr float kMin = 0.1f;
        constexpr float kMax = 60.0f;

        if (!(value >= kMin))
            return kMin;
        if (value >= kMax)
            return kMax;
        return static_cast<float>(static_cast<unsigned int>(value * 10.0f + 0.5f)) * 0.1f;
    }

    CONFIG_VARIABLE(Enabled, bool, false);
    constexpr HueVariableParams kTrajectoryHue{.min = 0, .max = 359, .def = 0};
    constexpr HueVariableParams kBounceHue{.min = 0, .max = 359, .def = 120};

    CONFIG_VARIABLE_HUE(TrajectoryHue, kTrajectoryHue);
    CONFIG_VARIABLE_HUE(BounceHue, kBounceHue);

    CONFIG_VARIABLE(AlwaysShowLastCache, bool, false);

    constexpr RangeConstrainedVariableParams<float> kCacheDuration{.min = 0.1f, .max = 60.0f, .def = 0.1f};
    CONFIG_VARIABLE_RANGE(CacheDuration, kCacheDuration);
}
