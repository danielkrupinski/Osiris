#pragma once

#define CONFIG_VARIABLE(name, type, defaultValue) \
struct name { \
    using ValueType = type; \
    static constexpr ValueType kDefaultValue = defaultValue; \
}

template <typename ConfigVariable>
struct UnpackConfigVariable {
    using type = typename ConfigVariable::ValueType;
};
