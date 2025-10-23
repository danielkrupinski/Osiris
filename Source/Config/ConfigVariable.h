#pragma once

#include <Utils/ColorUtils.h>
#include <Utils/IdentityMacro.h>
#include <Utils/InRange.h>

#define CONFIG_VARIABLE(name, type, defaultValue) \
struct name { \
    using ValueType = type; \
    static constexpr ValueType kDefaultValue{defaultValue}; \
}

template <typename ConfigVariable>
struct UnpackConfigVariable {
    using type = typename ConfigVariable::ValueType;
};

#define CONFIG_VARIABLE_HUE(name, hueParams) \
CONFIG_VARIABLE(name, \
    IDENTITY(InRange<color::HueInteger, color::HueInteger{hueParams.min}, color::HueInteger{hueParams.max}>), \
    color::HueInteger{hueParams.def})

#define CONFIG_VARIABLE_RANGE(name, params) \
CONFIG_VARIABLE(name, IDENTITY(InRange<decltype(params)::Type, params.min, params.max>), params.def)
