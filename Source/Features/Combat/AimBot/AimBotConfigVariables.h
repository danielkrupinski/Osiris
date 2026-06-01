#pragma once

#include <cstdint>

#include <Config/ConfigVariable.h>
#include <Config/RangeConstrainedVariableParams.h>

namespace aim_bot_vars
{

constexpr std::uint16_t kAltKey = 0x12;
constexpr std::uint16_t kMouse4Key = 0x05;
constexpr std::uint16_t kShiftKey = 0x10;

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(ActivationKey, std::uint16_t, kAltKey);
CONFIG_VARIABLE(TeamCheck, bool, true);

constexpr RangeConstrainedVariableParams<std::uint16_t> kFovPixels{.min = 10, .max = 500, .def = 120};
constexpr RangeConstrainedVariableParams<std::uint16_t> kMaxDistance{.min = 100, .max = 5000, .def = 2000};
constexpr RangeConstrainedVariableParams<std::uint8_t> kSmooth{.min = 1, .max = 30, .def = 6};
constexpr RangeConstrainedVariableParams<std::uint16_t> kVerticalAdjustment{.min = 0, .max = 120, .def = 64};
constexpr RangeConstrainedVariableParams<std::uint8_t> kDeadzonePixels{.min = 0, .max = 30, .def = 2};

CONFIG_VARIABLE_RANGE(FovPixels, kFovPixels);
CONFIG_VARIABLE_RANGE(MaxDistance, kMaxDistance);
CONFIG_VARIABLE_RANGE(Smooth, kSmooth);
CONFIG_VARIABLE_RANGE(VerticalAdjustment, kVerticalAdjustment);
CONFIG_VARIABLE_RANGE(DeadzonePixels, kDeadzonePixels);

}
