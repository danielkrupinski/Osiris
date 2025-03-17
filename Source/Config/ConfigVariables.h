#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>

#include "ConfigVariable.h"
#include "ConfigVariableTypes.h"
#include <Utils/Meta.h>

class ConfigVariables {
public:
    ConfigVariables() noexcept
    {
        ConfigVariableTypes::forEach([this] <typename ConfigVariable> (std::type_identity<ConfigVariable>) {
            storeVariableValue<ConfigVariable>(ConfigVariable::kDefaultValue);
        });
    }

    template <typename ConfigVariable>
    [[nodiscard]] auto getVariableValue() noexcept
    {
        if constexpr (OneByteConfigVariables::contains<ConfigVariable>()) {
            return std::bit_cast<typename ConfigVariable::ValueType>(oneByteConfigVariables[OneByteConfigVariables::template indexOf<ConfigVariable>()]);
        }
    }

    template <typename ConfigVariable>
    void storeVariableValue(ConfigVariable::ValueType value) noexcept
    {
        if constexpr (OneByteConfigVariables::contains<ConfigVariable>()) {
            std::memcpy(&oneByteConfigVariables[OneByteConfigVariables::template indexOf<ConfigVariable>()], &value, sizeof(value));
        }
    }

private:
    using OneByteConfigVariables = ConfigVariableTypes::filter<Projected<UnpackConfigVariable, WithSizeOf<1>::Equal>::Value>;

    std::array<std::byte, OneByteConfigVariables::size()> oneByteConfigVariables;
};
