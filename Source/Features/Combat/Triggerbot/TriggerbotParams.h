#pragma once

#include <cstdint>
#include <Config/RangeConstrainedVariableParams.h>

namespace triggerbot_params
{

constexpr auto kFireCooldownMs = RangeConstrainedVariableParams<std::uint16_t>{.min = 50, .max = 500, .def = 120};
constexpr auto kPixelRadius = RangeConstrainedVariableParams<std::uint8_t>{.min = 20, .max = 200, .def = 60};

}
