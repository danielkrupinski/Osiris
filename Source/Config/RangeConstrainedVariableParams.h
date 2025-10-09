#pragma once

#include <Utils/ColorUtils.h>

template <typename T>
struct RangeConstrainedVariableParams {
    using Type = T;

    T min, max, def;
};

using HueVariableParams = RangeConstrainedVariableParams<color::HueInteger::UnderlyingType>;
