#pragma once

#include <cstddef>
#include <type_traits>

#include <Config/ConfigVariableTypes.h>

struct MockConfig {
    MOCK_METHOD(bool, getVariableBool, (std::size_t configVariableTypeIndex));

    template <typename ConfigVariable>
    [[nodiscard]] auto getVariable() noexcept
    {
        if constexpr (std::is_same_v<typename ConfigVariable::ValueType, bool>) {
            return getVariableBool(ConfigVariableTypes::indexOf<ConfigVariable>());
        } else {
            static_assert(!std::is_same_v<ConfigVariable, ConfigVariable>, "Unknown type");
        }
    }
};
