#pragma once

#include <Config/RangeConstrainedVariableParams.h>

namespace triggerbot_params
{

constexpr auto kRadius = RangeConstrainedVariableParams<std::uint8_t>{.min = 50, .max = 255, .def = 150};

}
